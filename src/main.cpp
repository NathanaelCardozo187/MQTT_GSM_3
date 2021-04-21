#include <Arduino.h>

#define SerialAT Serial2
#define SerialMon Serial




#ifndef LED_BUILTIN
  
  #define LED_BUILTIN   2
#endif



unsigned int Counter = 0;
unsigned long datalength, CheckSum, RLength;
unsigned short topiclength;
unsigned char topic[30];
char str[250];
unsigned char encodedByte;
int X;



unsigned short MQTTProtocolNameLength;
unsigned short MQTTClientIDLength;
unsigned short MQTTUsernameLength;
unsigned short MQTTPasswordLength;


const char MQTTHost[30] = "spiraldevnew.sml.mx";
const char MQTTPort[10] = "1883";
const char MQTTClientID[20] = "SMLOO1";
const char MQTTTopic[30] = "AEREADOR";
const char MQTTProtocolName[10] = "MQIsdp";
const char MQTTLVL = 0x03;
const char MQTTFlags = 0xC2;
const unsigned int MQTTKeepAlive = 60; /////////////////////
const char MQTTUsername[30] = "testiosa";
const char MQTTPassword[35] = "sml6688154252";
const char MQTTQOS = 0x00;
const char MQTTPacketID = 0x0001;




void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  SerialAT.begin(115200);
  Serial.println(" MQTT prueba");
  
  delay(5000);
  
}




void SendConnectPacket(void) {
  SerialAT.print("\r\nAT+CIPSEND\r\n");
  delay(3000);
  SerialAT.write(0x10);
  MQTTProtocolNameLength = strlen(MQTTProtocolName);
  MQTTClientIDLength = strlen(MQTTClientID);
  MQTTUsernameLength = strlen(MQTTUsername);
  MQTTPasswordLength = strlen(MQTTPassword);
  datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 + MQTTUsernameLength + 2 + MQTTPasswordLength + 2;
  
  X = datalength;
  do {
    encodedByte = X % 128;
    X = X / 128;
    if (X > 0) {
      encodedByte |= 128;
    }
    SerialAT.write(encodedByte);
  }
  while (X > 0);
  SerialAT.write(MQTTProtocolNameLength >> 8);
  SerialAT.write(MQTTProtocolNameLength & 0xFF);
  SerialAT.print(MQTTProtocolName);
  SerialAT.write(MQTTLVL); // LVL
  SerialAT.write(MQTTFlags); // Flags
  SerialAT.write(MQTTKeepAlive >> 8);
  SerialAT.write(MQTTKeepAlive & 0xFF);
  SerialAT.write(MQTTClientIDLength >> 8);
  SerialAT.write(MQTTClientIDLength & 0xFF);
  SerialAT.print(MQTTClientID);
  SerialAT.write(MQTTUsernameLength >> 8);
  SerialAT.write(MQTTUsernameLength & 0xFF);
  SerialAT.print(MQTTUsername);
  SerialAT.write(MQTTPasswordLength >> 8);
  SerialAT.write(MQTTPasswordLength & 0xFF);
  SerialAT.print(MQTTPassword);
  SerialAT.write(0x1A);
}



void SendPublishPacket(void) {
  SerialAT.print("\r\nAT+CIPSEND\r\n");
  delay(3000);
  memset(str, 0, 250);
  topiclength = sprintf((char * ) topic, MQTTTopic);
  datalength = sprintf((char * ) str, "%s%u", topic, Counter);
  delay(1000);
  Serial.write(0x30);
  X = datalength + 2;
  do {
    encodedByte = X % 128;
    X = X / 128;
    if (X > 0) {
      encodedByte |= 128;
    }
    SerialAT.write(encodedByte);
  }
  while (X > 0);
  SerialAT.write(topiclength >> 8);
  SerialAT.write(topiclength & 0xFF);
  SerialAT.print(str);
  SerialAT.write(0x1A);
}





void SendSubscribePacket(void) {
  SerialAT.print("\r\nAT+CIPSEND\r\n");
  delay(3000);
  memset(str, 0, 250);
  topiclength = strlen(MQTTTopic);
  datalength = 2 + 2 + topiclength + 1;
  delay(2000);
  Serial.write(0x82);
  X = datalength;
  do {
    encodedByte = X % 128;
    X = X / 128;
    if (X > 0) {
      encodedByte |= 128;
    }
    SerialAT.write(encodedByte);
  }
  while (X > 0);
  SerialAT.write(MQTTPacketID >> 8);
  SerialAT.write(MQTTPacketID & 0xFF);
  SerialAT.write(topiclength >> 8);
  SerialAT.write(topiclength & 0xFF);
  SerialAT.print(MQTTTopic);
  SerialAT.write(MQTTQOS);
  SerialAT.write(0x1A);
}







void loop() {
  
  SerialAT.print("AT+CSTT=\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\"\r\n");
  SerialAT.print("AT+CIPMODE=0\r\n");
  delay(1000);
  SerialAT.print("AT+CIICR\r\n");
  delay(3000);
  SerialAT.print("AT+CIPSTART=\"TCP\",\"spiraldevnew.sml.mx\",\"1883\"\r\n");
  delay(5000);

  SendConnectPacket();
  delay(5000);

  
  SendSubscribePacket();
  while (1) {
    if (SerialAT.available() > 0) {
      str[0] = SerialAT.read();
      SerialAT.write(str[0]);
      if (str[0] == '1')
        digitalWrite(LED_BUILTIN, HIGH);
      if (str[0] == '0')
        digitalWrite(LED_BUILTIN, LOW);
    }
  }
}