#include "CentralBLE.h"
#include <bluefruit.h>
#include <queue>

namespace std {
void __throw_length_error(char const*) {}
void __throw_bad_alloc() {}
}

void CentralBLE::addPrphName(int ID, char *name)
{
  if ( ID < 0 ) return;
  prph_info_t* peer = &prphs[ID];
  strncpy(peer->name, name, sizeof(peer->name) - 1); //sizeof(peer->name) is 16+1=17
  peer->name[sizeof(peer->name) - 1] = '\0';
  //name の長さが 16+1 以上のときには 16+1 文字をコピー,このとき配列の範囲外なのでNULLが付けられない
  //name の長さが 16+1 より少ない場合には残りの文字をNULLで埋める
  //sizeof(peer->name)-1 コピーして最後をNULLにする
}

void CentralBLE::begin(int maxPrphConn, char *name)
{
  Bluefruit.autoConnLed(false);
  Bluefruit.begin(0, maxPrphConn);
  Bluefruit.setTxPower(4); //+4dBm for nRF52832, +8dBm for nRF52840
  Bluefruit.setName(name);
  for (uint8_t idx = 0; idx < sizeof(prphs) / sizeof(prph_info_t); idx++) //Init peripheral pool
  {
    prphs[idx].conn_handle = BLE_CONN_HANDLE_INVALID; //Invalid all connection handle
    prphs[idx].bleuart.begin(); //All of BLE Central Uart Serivce
    prphs[idx].bleuart.setRxCallback(bleuart_rx_callback);
    prphs[idx].CPS.begin();
    prphs[idx].CPM.setNotifyCallback(notify_callback);
    prphs[idx].CPM.begin();
  }
  Bluefruit.Central.setConnectCallback(connect_callback); //Callbacks for Central
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);

  //Scan Setting
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.filterRssi(-80);
  Bluefruit.Scanner.setInterval(160, 80);
  Bluefruit.Scanner.useActiveScan(true);
  Bluefruit.Scanner.start(0);
}

void CentralBLE::scan_callback(ble_gap_evt_adv_report_t* report)
{
  uint8_t buffer[32];
  memset(buffer, 0, sizeof(buffer)); //strlenだとbufferの初期値は0なのでNULLが見つからずエラーになる

  if (report->type.scan_response)
  {
    /*
      DEBUG_print("ScanResponce received from : ");
      DEBUG_printBufferReverse(report->peer_addr.addr, 6, ' '); //MAC is in little endian --> print reverse
      DEBUG_println();

      DEBUG_print("RSSI : ");
      DEBUG_print(report->rssi);
      DEBUG_println(" dBm");
    */
    if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
    {
      /*
        Serial.print("ScanResponce NAME : ");
        for (int i = 0; i < sizeof(buffer); i++) {
        Serial.print(char(buffer[i]));
        }
        Serial.println();
      */

      for (int i = 0; i < sizeof(prphs) / sizeof(prph_info_t); i++)
      {
        if (!memcmp(buffer, prphs[i].name, strlen(prphs[i].name) + 1)) //終端NULLまで含めて比較
        {
          if (Bluefruit.Central.connect(report))
          {
            //DEBUG_println("NAME matched. Connection established.");
            return; //NOTE:接続に成功したら一旦scanは停止
            //必須デバイスが揃ったかconnect_callbackの中で判断してstart(0)するか決める
            //必須デバイスが揃えば全て揃ってなくてもscanを完了させようと思ったが,
            //notifyを受信しながらでもscanできてしまうことが判明したので全て揃うまでscanを止めないようにする
          }
          else
          {
            //DEBUG_println("NAME matched. Connection failed.");
            //Continue Scanning
          }
        }
      }
      //DEBUG_println("No Connectable peripheral with matched NAME found.");
    }
    else
    {
      //DEBUG_println("No COMPLETE LOCAL NAME found.");
    }
  }
  else
  {
    //do nothing for advertise.
  }

  Bluefruit.Scanner.resume();
  //bleuartすらない謎デバイスを見つけてScanが止まると困るのでscan_callback内にresumeは必要
}

void CentralBLE::connect_callback(uint16_t conn_handle)
{
  char buffer[16 + 1];
  int id;
  prph_info_t* peer;

  Bluefruit.Connection(conn_handle)->getPeerName(buffer, sizeof(buffer) - 1); //bufferに謎の文字列が付加される問題,サンプルコードもこうなってるし多分この行のせいではない.

  //Serial.print("Connected NAME : ");
  //Serial.println(buffer);

  for (id = 0; id < sizeof(prphs) / sizeof(prph_info_t); id++)
  {
    peer = &prphs[id]; //アドレス渡し
    if ((!memcmp(buffer, peer->name, strlen(peer->name))) && (peer->conn_handle == BLE_CONN_HANDLE_INVALID))
    { //終端NULLまで含めた名前の一致と,まだconn_handleが設定されていないことが条件(同姓同名対策)
      //strlen(peer->name) + 1だったが,bufferに謎の文字列が付加されるので,NULLまでの比較はしない
      peer->conn_handle = conn_handle;
      break; //一致した時点でのidを以降使うためにbreak
    }
  }

  if (peer->CPS.discover(conn_handle) && peer->CPM.discover()) //CPS
  {
    peer->CPM.enableNotify();
  }
  else if (peer->bleuart.discover(conn_handle)) //NUS
  {
    peer->bleuart.enableTXD();
  }
  else //No service found
  {
    Bluefruit.disconnect(conn_handle);
  }

  Bluefruit.Scanner.start(0);
}

void CentralBLE::disconnect_callback(uint16_t conn_handle, uint8_t reason) //実装済
{
  (void) conn_handle;
  (void) reason; //reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h

  int id  = findConnHandle(conn_handle);
  if ( id < 0 ) return; //Non-existant connection, something went wrong, DBG !!!

  prphs[id].conn_handle = BLE_CONN_HANDLE_INVALID;

  //Serial.print(prphs[id].name);
  //Serial.println(" disconnected!");
}

void CentralBLE::notify_callback(BLEClientCharacteristic * chr, uint8_t* data, uint16_t len)
{
  //timestamp = word(data[7], data[6]); //単位は1/2048[s]じゃないっぽい
  //revolution = word(data[5], data[4]);
  //power = word(data[3], data[2]);
  //frag = word(data[1], data[0]);

  uint16_t conn_handle = chr->connHandle();
  int id = findConnHandle(conn_handle);
  prph_info_t* peer = &prphs[id];

  memcpy(peer->CPMpkt.bin, data, 8);

  if (peer->CPMpkt.revolution > peer->CPMpktprev.revolution)
  {
    //cadence = 60.0 / ( ((timestamp - timestamp_last) / 1024.0) / (revolution - revolution_last) );
    peer->cadence = 60 * 1024 * (peer->CPMpkt.revolution - peer->CPMpktprev.revolution) / (peer->CPMpkt.timestamp - peer->CPMpktprev.timestamp);
  }

  memcpy(peer->CPMpktprev.bin, peer->CPMpkt.bin, 8);

  //timestamp_last = timestamp;
  //revolution_last = revolution;
}

void CentralBLE::bleuart_rx_callback(BLEClientUart& uart_svc) //uart_svc is prphs[conn_handle].bleuart
{
  uint16_t conn_handle = uart_svc.connHandle();
  int id = findConnHandle(conn_handle);
  prph_info_t* peer = &prphs[id];
  //Serial.printf("[From %s]: ", peer->name); //Print sender's name
  while (uart_svc.available()) //uart_svc Reference object to the service where the data arrived.
  {
    char buf[20 + 1] = { 0 }; //default MTU with an extra byte for string terminator
    if ( uart_svc.read(buf, sizeof(buf) - 1) )
    {
      //Serial.println(buf);
      for (int i = 0; i <= 20; i++)
      {
        peer->FIFO.push(buf[i]);
      }
    }
  }
}

void CentralBLE::print(int ID, const char *message) //実装済
{
  if ( Bluefruit.Central.connected() ) //First check if we are connected to any peripherals
  {
    prph_info_t* peer = &prphs[ID];
    if ( peer->bleuart.discovered() )
    {
      peer->bleuart.print(message);
    }
  }
}

int CentralBLE::available(int ID) //実装済
{
  return prphs[ID].FIFO.size();
}

char CentralBLE::read(int ID) //実装済
{
  char tmp = prphs[ID].FIFO.front();
  prphs[ID].FIFO.pop();
  return tmp;
}

String CentralBLE::readStringUntil(int ID, char terminator) //実装済
{
  String str = "";
  while (1) {
    while (prphs[ID].FIFO.size() > 0)
    {
      char tmp = prphs[ID].FIFO.front();
      prphs[ID].FIFO.pop();
      if (tmp == terminator)
      {
        return str;
      }
      str += String(tmp);
      //delay(50);
    }
    delay(1);
  }
  return "error";
}

int16_t CentralBLE::getCyclingPower(int ID)
{
  return prphs[ID].CPMpkt.power;
}
uint16_t CentralBLE::getCyclingCadence(int ID)
{
  return prphs[ID].cadence;
}

int CentralBLE::findConnHandle(uint16_t conn_handle)  //実装済
{
  for (int id = 0; id < sizeof(prphs) / sizeof(prph_info_t); id++)
  {
    if (conn_handle == prphs[id].conn_handle)
    {
      return id; //return array index if found, otherwise -1
    }
  }
  return -1;
}

bool CentralBLE::isOpen(int ID) //実装済
{
  if (prphs[ID].conn_handle == BLE_CONN_HANDLE_INVALID)
  {
    return 0;
  }
  return 1;
}
