//#include <Adafruit_TinyUSB.h>

#include "StringSplitter.h"

#include "CentralBLE.h"
#include "TMUX131.h"

CentralBLE SerialBLE;
TMUX131 MUX;

#define LEFTPWR  0
#define RIGHTPWR 1
#define DISPLAY  2

void setup(void)
{
  Serial.begin(38400);
  //while (!Serial); yield();

  SerialBLE.addPrphName(LEFTPWR, "ASSIOMA33173U");
  SerialBLE.addPrphName(RIGHTPWR, "PowerMeter");
  SerialBLE.addPrphName(DISPLAY, "DISPLAY");

  SerialBLE.begin(3, "Central");

  MUX.EnableSender();
  //MUX.EnableUSB();
}

union PACKET
{
  struct {
    uint8_t header = 255;//1

    uint8_t PitotBatt;   //1
    int8_t PitotRssi;    //1
    bool PitotConn;      //1

    float PitotSpeed;    //4
    float PitotRoll;     //4
    float PitotPitch;    //4
    float PitotYaw;      //4

    uint8_t PowerBatt[2];//2
    int8_t PowerRssi[2]; //2
    bool PowerConn[2];   //2

    int16_t PowerAvg[2]; //2+2
    int16_t PowerMax[2]; //2+2
    uint16_t Cadence;    //2
  };
  uint8_t bin[36];
};
PACKET packet;

unsigned long LastTime = 0;

void loop()
{
  if (SerialBLE.isOpen(LEFTPWR))
  {
    packet.PowerConn[LEFTPWR] = 1;
    packet.Cadence = SerialBLE.getCyclingCadence(LEFTPWR);
    packet.PowerAvg[LEFTPWR] = SerialBLE.getCyclingPower(LEFTPWR) * 2; //double power here
    packet.PowerMax[LEFTPWR] = 0;
    packet.PowerBatt[LEFTPWR] = 100; //battery serviceを実装する。
  }
  else
  {
    packet.PowerConn[LEFTPWR] = 0;
  }

  if (SerialBLE.isOpen(RIGHTPWR)) {
    packet.PowerConn[RIGHTPWR] = 1;
    String tmp = SerialBLE.readStringUntil(RIGHTPWR, ',');
    StringSplitter *PowerData = new StringSplitter(tmp, ' ', 4);
    packet.Cadence = PowerData->getItemAtIndex(0).toInt();
    packet.PowerAvg[RIGHTPWR] = PowerData->getItemAtIndex(1).toInt();
    packet.PowerMax[RIGHTPWR] = PowerData->getItemAtIndex(2).toInt();
    packet.PowerBatt[RIGHTPWR] = 100; //電圧値を百分率に変換するか、パワメにbattery serviceを実装する。
  }
  else
  {
    packet.PowerConn[RIGHTPWR] = 0;
  }

  if (Serial.read() != -1)
  {
    MUX.EnableReceiver();
    delay(50);
    Serial.write(packet.bin, sizeof(PACKET)); //36bytes
    Serial.flush();
    delay(50);
    MUX.EnableSender();
  }

  if ((SerialBLE.isOpen(DISPLAY)) && (millis() - LastTime > 1000))
  {
    String str = String(packet.Cadence);
    str += " ";
    str += String(packet.PowerAvg[LEFTPWR]); //doubled
    str += " ";
    str += String(packet.PowerBatt[LEFTPWR]); //battery serviceを実装するまでは0しか出ない、実装したらディスプレイ側に変更が必要
    str += " ";
    str += String(0);//int(packet.AirSpeed * 100.0)
    str += " ";
    str += String(packet.PitotBatt);
    str += ",";
    SerialBLE.print(DISPLAY, str.c_str());

    LastTime = millis();
  }

  //delay(10);
}
