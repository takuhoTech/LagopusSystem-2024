//#include <Adafruit_TinyUSB.h>

#include "StringSplitter.h"

#include "CentralBLE.h"
#include "TMUX131.h"

CentralBLE SerialBLE;
TMUX131 MUX;

void setup(void)
{
  MUX.EnableUSB();

  Serial.begin(115200);
  while (!Serial); yield();

  //Serial.println("BLE Central Example");

  SerialBLE.addPrphName(1, "ASSIOMA33173U");
  SerialBLE.addPrphName(0, "DISPLAY");
  SerialBLE.addPrphName(2, "PowerMeter");
  SerialBLE.begin(3, "Central");

  Serial.println("Central started");

}

void loop()
{
  if (SerialBLE.isOpen(1))
  {
    Serial.print("ASSIOMA: ");
    Serial.print(SerialBLE.getCyclingCadence(1));
    Serial.print(" RPM ");
    Serial.print(SerialBLE.getCyclingPower(1));
    Serial.print(" W ");

  }

  if (SerialBLE.isOpen(0))
  {
    //Serial.println("Display OK");
    String str = String(SerialBLE.getCyclingCadence(1));
    str += " ";
    str += String(SerialBLE.getCyclingPower(1));
    str += " ";
    str += String(420);
    str += " ";
    str += String(0);
    str += " ";
    str += String(420);
    str += ",";
    SerialBLE.print(0, str.c_str());
  }

  if (SerialBLE.isOpen(2)) {
    String tmp = SerialBLE.readStringUntil(2, ',');
    StringSplitter *PowerData = new StringSplitter(tmp, ' ', 4);

    Serial.print("LAGOPUS: ");
    Serial.print(PowerData->getItemAtIndex(0).toInt());
    Serial.print(" RPM ");
    Serial.print(PowerData->getItemAtIndex(1).toInt());//doubled AVG
    Serial.print(" W ");
    Serial.print(PowerData->getItemAtIndex(2).toInt());//doubled MAX
    Serial.print(" W ");
    Serial.print(PowerData->getItemAtIndex(3).toFloat());//BAT
    Serial.print(" V ");
  }

  Serial.println();

  delay(500);

  /*
    union PACKET
    {
    struct {
      bool AirMeterIsOpen = 0;  //1byte
      float AirSpeed;      //4byte
      float AirMeterBat;   //4byte
      bool PowerMeterIsOpen = 0; //1byte
      uint16_t Cadence;    //2byte
      uint16_t PowerAvg;   //2byte
      uint16_t PowerMax;   //2byte
      float PowerMeterBat; //4byte
    };
    uint8_t bin[20];
    };
    PACKET packet;

    delay(500);

    while (1) {
    if (SerialBLE.isOpen(0)) {
      packet.AirMeterIsOpen = 1;
      //digitalWrite(LED_RED, HIGH);
      String tmp = SerialBLE.readStringUntil(0, ',');
      StringSplitter *AirData = new StringSplitter(tmp, ' ', 2);
      packet.AirSpeed = AirData->getItemAtIndex(0).toFloat();
      packet.AirMeterBat = AirData->getItemAtIndex(1).toFloat();
    }
    else
    {
      packet.AirMeterIsOpen = 0;
      //digitalWrite(LED_RED, LOW);
    }
    if (SerialBLE.isOpen(3)) {
      packet.PowerMeterIsOpen = 1;
      //digitalWrite(LED_BLUE, HIGH);
      String tmp = SerialBLE.readStringUntil(3, ',');
      StringSplitter *PowerData = new StringSplitter(tmp, ' ', 4);
      packet.Cadence = PowerData->getItemAtIndex(0).toInt();
      packet.PowerAvg = PowerData->getItemAtIndex(1).toInt();
      packet.PowerMax = PowerData->getItemAtIndex(2).toInt();
      packet.PowerMeterBat = PowerData->getItemAtIndex(3).toFloat();
    }
    else
    {
      packet.PowerMeterIsOpen = 0;
      //digitalWrite(LED_BLUE, LOW);
    }
    if (Serial1.read() != -1)
    {
      Serial1.write(packet.bin, sizeof(PACKET));
    }

    if (SerialBLE.isOpen(2)) {
      String str = String(packet.Cadence);
      str += " ";
      str += String(packet.PowerAvg);
      str += " ";
      str += String(int(packet.PowerMeterBat * 100.0));
      str += " ";
      str += String(int(packet.AirSpeed * 100.0));
      str += " ";
      str += String(int(packet.AirMeterBat * 100.0));
      str += ",";
      SerialBLE.print(2, str.c_str());
    }
    delay(100);
    }
  */
}
