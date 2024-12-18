#include <bluefruit.h>
#include "Adafruit_SPIFlash.h"

Adafruit_FlashTransport_QSPI flashTransport;

void deep_sleep()
{
  // Turn of flash, has like 15µA consumption.
  flashTransport.begin();
  flashTransport.runCommand(0xB9);
  flashTransport.end();

  // wake pin from deep sleep
  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[2], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

  // set deep sleep power off mode.
  NRF_POWER->SYSTEMOFF = 1;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(1000);

  // Normal mode
  while(!digitalRead(2)) //Wait for pin change
  {
    //delay(1); //normal
    delay(500); //normal modified
  }
  // End normal mode

  // deep sleep mode
  deep_sleep(); //Wake-up based on external pin change
  // End deep sleep mode
}
void loop()
{
}
