#include "Wire.h"

#define I2C_DEV_ADDR 0x12

uint32_t i = 0;

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Wire.begin();

  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(2000);

  //Write message to the slave
  Wire.beginTransmission(I2C_DEV_ADDR);
  Wire.write(0);
  Wire.write(0x21);
  Wire.write(100); //10 sec scan time
  Wire.write(0);
  Wire.write(20);
  Wire.write(0);
  Wire.write(0);
  Wire.write(21);
  Wire.write(0);
  Wire.write(22);
  Wire.write(0);
  Wire.write(0);
  Wire.write(0);
  uint8_t error = Wire.endTransmission(true);
  Serial.print("endTransmission");

  delay(10000 + 200);


  //Read all scan bytes from the slave
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, 10);
  Serial.print("requestFrom: ");
  Serial.println(bytesReceived);
  if ((bool)bytesReceived) {  //If received more than zero bytes
    uint8_t temp[bytesReceived];
    Wire.readBytes(temp, bytesReceived);
    //log_print_buf(temp, bytesReceived);
  }
}
