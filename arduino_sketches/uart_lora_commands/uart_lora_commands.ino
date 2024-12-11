#include<HardwareSerial.h>

HardwareSerial SUART(2);

#define RX    16
#define TX    17
#define BAUD  9600

const String DevAddr = "260B2A9A";
const String APPSKEY = "2E4BBC85B2AAB5B31FCC5BB0A16B07BC";
const String NWKSKEY = "28D5A8BCD4F80E9DFE8640F5B7072C49";
const String DevEui  = "70B3D57ED006C5CD";

void setup() {
  Serial.begin(115200);
  while (!Serial) { } // wait for serial port to connect. Needed for native USB port only
  SUART.begin(BAUD, SERIAL_8N1, RX, TX);

  sendSettings();
  //sendCommand("AT+msg=\"Tom\""); receiveFeedBack();
  delayMicroseconds(100);
  //sendCommand("AT+MSGHEX=\"0F 1E 2D 3C 4B 5A 96 87\""); receiveFeedBack();
  delayMicroseconds(100);
}

void loop() {
   /*if (SUART.available()) {
    Serial.println(SUART.read(), HEX);
  }*/
}

void sendCommand(String command) {
  // Send connection check
  SUART.print(command+"\n");
  receiveFeedBack();
}
void receiveFeedBack() {
  // Read response
  while (!SUART.available());
  String output = SUART.readString();
  if (output.indexOf('+') != -1) output = output.substring(output.indexOf('+'));
  Serial.print(output);
}

void sendSettings() {
  sendCommand("AT");
  delayMicroseconds(10);
  sendCommand("AT+MODE=\"LWABP\"");
  delayMicroseconds(10);
  sendCommand("AT+DR=EU868");
  delayMicroseconds(10);
  sendCommand("AT+CH=num,0");
  delayMicroseconds(10);
  sendCommand("AT+ID=DevAddr,\""+DevAddr+"\"");
  delayMicroseconds(10);
  sendCommand("AT+KEY=APPSKEY,\""+APPSKEY+"\"");
  delayMicroseconds(10);
  sendCommand("AT+KEY=NWKSKEY,\""+NWKSKEY+"\"");
  delayMicroseconds(10);
  sendCommand("AT+ID=DevEui,\""+DevEui+"\"");
  delayMicroseconds(10);
  sendCommand("AT+ID=AppEui,\""+DevEui+"\"");
  delayMicroseconds(10);
  sendCommand("AT+MODE=\"LWABP\"");
  delayMicroseconds(10);
  sendCommand("AT+DR=7");
  delayMicroseconds(10);
  sendCommand("AT+CH=0");
  delayMicroseconds(10);
}
