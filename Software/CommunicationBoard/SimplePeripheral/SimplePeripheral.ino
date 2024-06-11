#include "PeripheralBLE.h"
#include "TMUX131.h"
//#include <Adafruit_TinyUSB.h>

PeripheralBLE SerialBLE;
TMUX131 SerialMUX;//pinMode constructor

void setup()
{
  Serial.begin(115200); while (!Serial) yield();
  SerialMUX.EnableReceiver(); //ISP4520 -> PICO
  Serial.println("Serial MUX test code");
  Serial.flush(); //Waits for the transmission of outgoing serial data to complete.
  SerialMUX.EnableSender(); //ISP4520 <- PICO

  SerialBLE.begin("ISP4520"); // Sleep functions need the softdevice to be active.
}

void loop()
{
  if (Serial.available())
  {
    SerialBLE.write(Serial.readStringUntil('\n').c_str());
  }

  /*String str = String(random(120));
    str += "RPM ";
    str += String(random(300));
    str += "W ";
    str += String(random(300));
    str += "W ";
    str += String(4.2);
    str += "V,";
    SerialBLE.write(str.c_str());*/

  delay(100);
}
