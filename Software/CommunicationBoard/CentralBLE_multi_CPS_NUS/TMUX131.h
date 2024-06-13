#ifndef TMUX131_h
#define TMUX131_h
#include "Arduino.h"

class TMUX131 {
  public:
    /*void begin(unsigned long) override
      {
      HardwareSerial::begin(unsigned long);
      };
      void begin(unsigned long baudrate, uint16_t config) override;
      void end() override;
      int available(void) override;
      int peek(void) override;
      int read(void) override;
      void flush(void) override;
      size_t write(uint8_t) override;
      size_t write(const uint8_t *buffer, size_t size) override;
      int availableForWrite(void) override;
      //using Print::write; // pull in write(str) from Print
      operator bool() override;*/
    TMUX131()
    {
      pinMode(SEL0, OUTPUT);
      pinMode(SEL1, OUTPUT);
    };
    void EnableSender();
    void EnableReceiver();
    void EnableUSB();
    void Disable();
  private:
    int SEL0 = 29;
    int SEL1 = 28;
};

#endif
