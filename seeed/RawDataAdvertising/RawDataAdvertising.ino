#include <ArduinoBLE.h>

BLEService myService("fff0");
//BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast);
BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast, 2, "1"); //Define a charactersitic that will be used in the property

// Advertising parameters should have a global scope. Do NOT define them in 'setup' or in 'loop'
uint8_t completeRawAdvertisingData[] = {0x02,0x01,0x06,0x09,0xff,0x01,0x01,0x00,0x01,0x02,0x03,0x04,0xFF};   

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  if (!BLE.begin()) {
    Serial.println("failed to initialize BLE!");
    while (1);
  }

  Serial.println(myCharacteristic.value());
  int buf = 0;
  myCharacteristic.readValue(&buf);
  Serial.println(buf);
  
  buf++;
  
  bleCharacteristic.writeValue(&buf);
  
  Serial.println(myCharacteristic.value());
  myCharacteristic.readValue(&buf);
  Serial.println(buf);
  
  myService.addCharacteristic(myCharacteristic);
  BLE.addService(myService);

  completeRawAdvertisingData[7] = 0x89;
  
  // Build advertising data packet
  BLEAdvertisingData advData;
  // If a packet has a raw data parameter, then all the other parameters of the packet will be ignored
  advData.setRawData(completeRawAdvertisingData, sizeof(completeRawAdvertisingData));  
  // Copy set parameters in the actual advertising packet
  BLE.setAdvertisingData(advData);

  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setLocalName("Bla kut");
  // Copy set parameters in the actual scan response packet
  BLE.setScanResponseData(scanData);
  
  BLE.advertise();

  Serial.println("advertising ...");
}

void loop() {
  BLE.poll();
  Serial.println("Polled!");
}
