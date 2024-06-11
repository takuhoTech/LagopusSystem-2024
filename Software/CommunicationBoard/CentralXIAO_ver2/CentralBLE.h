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
    static void bleuart_rx_callback(BLEClientUart& uart_svc);

    void print(int ID, const char *message);
    int available(int ID);
    char read(int ID);
    String readStringUntil(int ID, char terminator);

    static int findConnHandle(uint16_t conn_handle);
    bool isOpen(int ID);
};

typedef struct //Struct containing peripheral info
{
  char name[16 + 1];
  uint16_t conn_handle;
  BLEClientUart bleuart; //Each prph need its own bleuart client service
  std::queue<char> FIFO; //bleuart_rx_callbackの中でFIFO[受信したID]に受信内容を書き込む
} prph_info_t;
static prph_info_t prphs[BLE_MAX_CONNECTION];

#endif
