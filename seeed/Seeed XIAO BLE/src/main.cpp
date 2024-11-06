// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire.h>
void receiveEvent(int howMany);

#define trigger_pin D0

void setup()
{
    //Set all pins to high_float
    for(uint8_t i = 0; i <= 10; i++){
        pinMode(i, INPUT);
    }

    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);

    Wire.begin(0x10);             // join i2c bus with address 0x10
    Wire.onReceive(receiveEvent); // register event
    Serial.begin(115200);         // start serial for output
}

void loop()
{
    //Cycle through red and blue to indicate ready.
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
    while (1 < Wire.available()) // loop through all but the last
    {
        char c = Wire.read(); // receive byte as a character
        Serial.print(c);      // print the character
    }
    int x = Wire.read(); // receive byte as an integer
    Serial.println(x);   // print the integer
}