#ifndef CentralBLE_h
#define CentralBLE_h
#include "Arduino.h"
#include <bluefruit.h>
#include <queue>

class CentralBLE
{
  public:
    void addPrphName(int ID, char *name);
    void begin(int maxPrphConn, char *name);

    static void scan_callback(ble_gap_evt_adv_report_t* report);
    static void connect_callback(uint16_t conn_handle);
    static void disconnect_callback(uint16_t conn_handle, uint8_t reason);
    static void notify_callback(BLEClientCharacteristic * chr, uint8_t* data, uint16_t len);
    static void bleuart_rx_callback(BLEClientUart& uart_svc);

    void print(int ID, const char *message);
    int available(int ID);
    char read(int ID);
    String readStringUntil(int ID, char terminator);

    int16_t getCyclingPower(int ID);
    uint16_t getCyclingCadence(int ID);

    static int findConnHandle(uint16_t conn_handle);
    bool isOpen(int ID);
};

union prph_packet_cpm
{
  struct {
    uint16_t frag = 0;       //2byte
    int16_t power = 0;       //2byte
    uint16_t revolution = 0; //2byte
    uint16_t timestamp = 0;  //2byte
  };
  uint8_t bin[8];
};

typedef struct //Struct containing peripheral info
{
  char name[16 + 1];
  uint16_t conn_handle;

  BLEClientUart bleuart; //Each prph need its own bleuart client service
  std::queue<char> FIFO; //bleuart_rx_callbackの中でFIFO[受信したID]に受信内容を書き込む

  BLEClientService        CPS = BLEClientService(UUID16_SVC_CYCLING_POWER); //0x1818
  BLEClientCharacteristic CPM = BLEClientCharacteristic(UUID16_CHR_CYCLING_POWER_MEASUREMENT); //0x2A63
  prph_packet_cpm CPMpkt;
  prph_packet_cpm CPMpktprev;
  uint16_t cadence;

} prph_info_t;
static prph_info_t prphs[BLE_MAX_CONNECTION];

#endif
