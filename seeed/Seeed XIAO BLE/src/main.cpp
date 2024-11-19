// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.
#include "Arduino.h"
#include <Wire.h>

struct ble_module_data_t
{
    uint8_t ssr_id;          // The ID of the rover itself
    uint8_t beacon_time;     // How long the beacon may last (val*100=ms)
    uint8_t scan_time;       // How long the scan may last (val*100=ms)
    int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
    uint8_t env_humidity;    // Range from -0-100%
    uint16_t env_lux;        // Range from 0 to 1000
    uint16_t dev_voltage;    // Range from 0-6.5535V (val/10000=V) (val/10=mV)
    // x gyro 8bit?
    // y gyro 8bit?
    // z gyro 8bit?
};

struct ble_module_data_t data;

void receiveEvent(int howMany);

#define trigger_pin D0

void setup()
{
    // Set all pins to high_float
    for (uint8_t i = 0; i <= 10; i++)
    {
        pinMode(i, INPUT);
    }

    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);

    Wire.begin(0x12);             // join i2c bus with address 0x10
    Wire.onReceive(receiveEvent); // register event
    Serial.begin(115200);         // start serial for output
}

void loop()
{
    // Cycle through red and blue to indicate ready.
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDB, LOW);
    delay(500);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDB, HIGH);
    delay(500);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    uint8_t i = 0;
    Serial.println(howMany);
    while (Wire.available()) // loop through all but the last
    {
        uint8_t c = Wire.read(); // receive byte as a character

        Serial.print(i); // print the character
        Serial.print(" "); // print the character
        Serial.print(c); // print the character
        Serial.println(" "); // print the character

        if (i == 0) // ssr_id
            data.ssr_id = (uint8_t)c;
        if (i == 1) // beacon_time
            data.beacon_time = (uint8_t)c;
        if (i == 2) // scan_time
            data.scan_time = (uint8_t)c;
        if (i == 3) // env_temperature HSB
        {
            i++;
            uint8_t c1 = Wire.read();
            Serial.print(i); // print the character
            Serial.print(" "); // print the character
            Serial.print(c1); // print the character
            Serial.println(" "); // print the character
            
            data.env_temperature = (int16_t)(c << 8 | c1);
        }

        if (i == 5) // env_humidity
            data.env_humidity = (uint8_t)c;
        if (i == 6) // env_lux
        {
            i++;
            
            uint8_t c1 = Wire.read();
            Serial.print(i); // print the character
            Serial.print(" "); // print the character
            Serial.print(c1); // print the character
            Serial.println(" "); // print the character

            data.env_lux = (int16_t)(c << 8 | c1);
        }

        if (i == 8) // dev_voltage
        {   
            i++;
            uint8_t c1 = Wire.read();
            Serial.print(i); // print the character
            Serial.print(" "); // print the character
            Serial.print(c1); // print the character
            Serial.println(" "); // print the character

            data.dev_voltage = (uint16_t) (c << 8 | c1);
        }

        i++;
    }
    // int x = Wire.read(); // receive byte as an integer
    // Serial.println(x);   // print the integer

    //Print the whole struct
    Serial.print("id: ");
    Serial.print(data.ssr_id);
    Serial.print(" bt: ");
    Serial.print(data.beacon_time);
    Serial.print(" st: ");
    Serial.print(data.scan_time);
    Serial.print(" et: ");
    Serial.print(data.env_temperature);
    Serial.print(" eh: ");
    Serial.print(data.env_humidity);
    Serial.print(" el: ");
    Serial.print(data.env_lux);
    Serial.print(" dv: ");
    Serial.println(data.dev_voltage);

}

/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
 * This sketch demonstrate how to run both Central and Peripheral roles
 * at the same time. It will act as a relay between an central (mobile)
 * to another peripheral using bleuart service.
 *
 * Mobile <--> DualRole <--> peripheral Ble Uart
 */
// #include <ArduinoBLE.h>

// BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// // Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

// const int ledPin = LED_BUILTIN; // pin to use for the LED

// void setup() {
//   Serial.begin(9600);
//   //while (!Serial);

//   // set LED pin to output mode
//   pinMode(ledPin, OUTPUT);

//   // begin initialization
//   if (!BLE.begin()) {
//     Serial.println("starting Bluetooth® Low Energy module failed!");

//     while (1);
//   }

//   // set advertised local name and service UUID:
//   BLE.setLocalName("LED");
//   BLE.setAdvertisedService(ledService);

//   // add the characteristic to the service
//   ledService.addCharacteristic(switchCharacteristic);

//   // add service
//   BLE.addService(ledService);

//   // set the initial value for the characeristic:
//   switchCharacteristic.writeValue(0);

//   // start advertising
//   BLE.advertise();

//   Serial.println("BLE LED Peripheral");
// }

// void loop() {
//   // listen for Bluetooth® Low Energy peripherals to connect:
//   BLEDevice central = BLE.central();

//   // if a central is connected to peripheral:
//   if (central) {
//     Serial.print("Connected to central: ");
//     // print the central's MAC address:
//     Serial.println(central.address());

//     // while the central is still connected to peripheral:
//   while (central.connected()) {
//         if (switchCharacteristic.written()) {
//           if (switchCharacteristic.value()) {
//             Serial.println("LED on");
//             digitalWrite(ledPin, LOW); // changed from HIGH to LOW
//           } else {
//             Serial.println(F("LED off"));
//             digitalWrite(ledPin, HIGH); // changed from LOW to HIGH
//           }
//         }
//       }

//     // when the central disconnects, print it out:
//     Serial.print(F("Disconnected from central: "));
//     Serial.println(central.address());
//   }
// }
