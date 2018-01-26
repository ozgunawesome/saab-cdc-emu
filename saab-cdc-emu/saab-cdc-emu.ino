#include <SPI.h>
#include "mcp_can.h"
#define DEBUGMODE 1
byte CDCcmd[] = {0xE0, 0x00, 0x3F, 0x31, 0xFF, 0xFF, 0xFF, 0xD0};
byte ninefivecmd[] = {0x32, 0x00, 0x00, 0x16, 0x01, 0x02, 0x00, 0x00};

int beep[] = {0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int playipod[] = {0xFF, 0x55, 0x04, 0x02, 0x00, 0x00, 0x01, 0xF9};
int playpauseipod[] = {0xFF, 0x55, 0x03, 0x02, 0x00, 0x01, 0xFA};
int stopipod[] = {0xFF, 0x55, 0x04, 0x02, 0x00, 0x00, 0x02, 0xF8};
int next[] = {0xFF, 0x55, 0x03, 0x02, 0x00, 0x08, 0xF3};
int prev[] = {0xFF, 0x55, 0x03, 0x02, 0x00, 0x10, 0xEB};
int shuffle[] = {0xFF, 0x55, 0x04, 0x02, 0x00, 0x00, 0x80, 0x7A};
int repeat[] = {0xFF, 0x55, 0x05, 0x02, 0x00, 0x00, 0x00, 0x01, 0xF8};
int buttonRelease[] = {0xFF, 0x55, 0x03, 0x02, 0x00, 0x00, 0xFB};

MCP_CAN myCan(10);

long unsigned int rxId;
byte len = 0;
byte rxBuf[8];
int error = 0;
unsigned long time, last_time;

void setup() {
  pinMode(2, INPUT);
  digitalWrite(A0, HIGH); //pl pu
  digitalWrite(A1, HIGH); //vol- / rw
  digitalWrite(A2, HIGH); //vol+ / ff
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A0, LOW); //pl pu
  digitalWrite(A1, LOW); //vol- / rw
  digitalWrite(A2, LOW); //vol+ / ff
  Serial.begin(9600);
  Serial.println("saabcdc emulator ozgun");
  time = 0;
  last_time = 1000;
  myCan.begin_setreg(0x43, 0xb6, 0x05);  // Saab I-Bus is 47.619kbps
  delay(10);
}

void receiveCanData()
{
  rxId = myCan.getCanId();                    // Get message ID
  if (rxId == 0x6a1)
    myCan.sendMsgBuf(0x6A2, 0, 8, ninefivecmd);
  else if (rxId == 0x290)
  {
    myCan.readMsgBuf(&len, rxBuf);
    //    if (rxBuf[0] == 0x00) return;
    digitalWrite(A0, rxBuf[2] & 0x04 > 0 ? HIGH : LOW);
    digitalWrite(A1, rxBuf[2] & 0x08 > 0 ? HIGH : LOW);
    digitalWrite(A2, rxBuf[2] & 0x10 > 0 ? HIGH : LOW);
  }
#if DEBUGMODE==1
  else
  {
    myCan.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++)
    {
      if (rxBuf[i] < 0x10) Serial.print("0");
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
#endif
}

void loop() {

  if (!digitalRead(2)) receiveCanData();
  if (Serial.available() > 0) {
    // read the incoming byte:
    uint8_t incomingByte = Serial.read();
    if (incomingByte >= 0x31 && incomingByte <= 0x33)
    {
      uint8_t pin = incomingByte - 35;
      Serial.print(" Toggle pin : ");
      Serial.print(pin, HEX);
      Serial.println();

      digitalWrite(pin, HIGH);
      delay(100);
      digitalWrite(pin, LOW);
      delay(100);

    }

  }

  time = millis();
  if (time - last_time > 700)
  {
    last_time = time;
    myCan.sendMsgBuf(0x3C8, 0, 8, CDCcmd);
  }
}

