#include <Arduino.h>
#include "Wire.h"

//https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/bluefruit.h

TwoWire *wi = &Wire;

void setup()
{
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);  // start serial for output
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  
  wi->begin();        // join i2c bus (address optional for main)
}

void loop()
{
  digitalWrite(LED_RED, HIGH);
  delay(1000);
  digitalWrite(LED_RED, LOW);
  delay(1000);

  for (int addr = 1; addr < 128; addr++)
  {
    wi->beginTransmission(addr);
    if ( 0 == wi->endTransmission() )
    {
      Serial.print("Found: 0x");
      Serial.print(addr, HEX);
      Serial.println();
    }
  }

  delay(1000);
}

