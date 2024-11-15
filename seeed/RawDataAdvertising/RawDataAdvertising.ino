#include <ArduinoBLE.h>


void blePeripheralConnectHandler(BLEDevice central);
void myCharacteristicRead(BLEDevice central, BLECharacteristic characteristic);

BLEService myService("fff0");
//BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast);

//Define a charactersitic that will be used in the property
BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast); 
BLECharacteristic myCounterChar("fff2", BLERead | BLEBroadcast, 4);


// Advertising parameters should have a global scope. Do NOT define them in 'setup' or in 'loop'
//Data from 7-12 is visible in advertisement
uint8_t completeRawAdvertisingData[] = {0x02,0x01,0x06,0x09,0xff,0x01,0x01,0x00,0x01,0x02,0x03,0x04,0xFF};   

uint8_t buf = 0;
void setup() {
  Serial.begin(9600);
  //while (!Serial);

  if (!BLE.begin()) {
    Serial.println("failed to initialize BLE!");
    while (1);
  }


  //Define the name of this device
  BLE.setDeviceName("The son");
  
  Serial.print("myCharacteristic.value(): ");
  Serial.println(myCharacteristic.value());
  buf = 0;
  myCharacteristic.readValue(&buf, sizeof(buf));
  Serial.print("buf ");
  Serial.println(buf);
  
  myCharacteristic.writeValue(buf);

  Serial.print("myCharacteristic.value(): ");
  Serial.println(myCharacteristic.value());
  myCharacteristic.readValue(&buf, sizeof(buf));
  Serial.print("buf ");
  Serial.println(buf);

  //Broadcast the characteristics value as service data when advertising
  myCharacteristic.broadcast();
  
  myService.addCharacteristic(myCharacteristic);
  BLE.addService(myService);

  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  myCharacteristic.setEventHandler(BLERead, myCharacteristicRead);

  

  //completeRawAdvertisingData[7] = 0x89;
  
  // Build advertising data packet
  //BLEAdvertisingData advData;
  
  // If a packet has a raw data parameter, then all the other parameters of the packet will be ignored
  //advData.setRawData(completeRawAdvertisingData, sizeof(completeRawAdvertisingData));  
  
  // Copy set parameters in the actual advertising packet
  //BLE.setAdvertisingData(advData);

  BLE.setAdvertisedServiceUuid("19B10000-E8F2-537E-4F6C-D104768A1214");


  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setLocalName("Jesus");
  // Copy set parameters in the actual scan response packet
  BLE.setScanResponseData(scanData);
  
  BLE.advertise();

  Serial.println("advertising ...");
}

void loop() {
  BLE.poll(); //Poll for Bluetooth® Low Energy radio events and handle them.### Syntax
  
  //Serial.println("Polled!");
  if(BLE.connected())
  {
    //Serial.println("Hoocked!");
    //When a device is connected
  }
  else
  {
    buf++;
    myCharacteristic.writeValue(buf);

    completeRawAdvertisingData[7] = buf;
    
    delay(1000);
  }
}

void blePeripheralConnectHandler(BLEDevice central)
{
  central.discoverAttributes();
  if(central.connected())
  {
    Serial.print("Connected to ");
    Serial.print("Dev name: ");
    Serial.println(central.deviceName());

    if (central.hasLocalName()) 
    {
      Serial.print("Local Name: ");
      Serial.println(central.localName());
    }
  }
}

void myCharacteristicRead(BLEDevice central, BLECharacteristic characteristic)
{
  central.discoverAttributes();
  if(central.connected())
  {
    Serial.print("Readed by ");
    Serial.print("Dev name: ");
    String name = central.deviceName();
    Serial.println(name);

    if (central.hasLocalName()) 
    {
      Serial.print("Local Name: ");
      Serial.println(central.localName());
    }
  }
}
