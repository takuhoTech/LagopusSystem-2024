//#include <Adafruit_TinyUSB.h>
#include "StringSplitter.h"
#include "CentralBLE.h"

CentralBLE SerialBLE;

void setup(void)
{
  Serial.begin(115200);
  while (!Serial); yield();

  SerialBLE.addPrphName(0, "AirMeter");
  SerialBLE.addPrphName(1, "ASSIOMA33173U");
  SerialBLE.addPrphName(2, "Display");
  SerialBLE.begin(3, "Central");
}

void loop()
{
  if (SerialBLE.isOpen(1))
  {
    Serial.print(SerialBLE.getCyclingCadence(1));
    Serial.print(" rpm ");
    Serial.print(SerialBLE.getCyclingPower(1));
    Serial.println(" w");
  }
  delay(250);

  /*  union PACKET
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
     if (SerialBLE.isOpen(AIRMETER)) {
       packet.AirMeterIsOpen = 1;
       digitalWrite(LED_RED, HIGH);
       String tmp = SerialBLE.readStringUntil(getPeripheralID(AIRMETER), ',');
       StringSplitter *AirData = new StringSplitter(tmp, ' ', 2);
       packet.AirSpeed = AirData->getItemAtIndex(0).toFloat();
       packet.AirMeterBat = AirData->getItemAtIndex(1).toFloat();
     }
     else
     {
       packet.AirMeterIsOpen = 0;
       digitalWrite(LED_RED, LOW);
     }
     if (SerialBLE.isOpen(POWERMETER)) {
       packet.PowerMeterIsOpen = 1;
       digitalWrite(LED_BLUE, HIGH);
       String tmp = SerialBLE.readStringUntil(getPeripheralID(POWERMETER), ',');
       StringSplitter *PowerData = new StringSplitter(tmp, ' ', 4);
       packet.Cadence = PowerData->getItemAtIndex(0).toInt();
       packet.PowerAvg = PowerData->getItemAtIndex(1).toInt();
       packet.PowerMax = PowerData->getItemAtIndex(2).toInt();
       packet.PowerMeterBat = PowerData->getItemAtIndex(3).toFloat();
     }
     else
     {
       packet.PowerMeterIsOpen = 0;
       digitalWrite(LED_BLUE, LOW);
     }
     if (Serial1.read() != -1)
     {
       Serial1.write(packet.bin, sizeof(PACKET));
     }

     if (SerialBLE.isOpen(ROUNDDISPLAY)) {
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
       SerialBLE.print(getPeripheralID(ROUNDDISPLAY), str.c_str());
     }
     delay(100);
    }*/
}
