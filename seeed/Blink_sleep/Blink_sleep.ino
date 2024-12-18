/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
#include <Adafruit_SPIFlash.h>


Adafruit_FlashTransport_QSPI flashTransport;

void deep_sleep();
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);



}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  //suspendLoop();
  deep_sleep();
}

void deep_sleep()
{
//  Bluefruit.end(); //Didn't tested yet
  
  Serial.println("Go to deep sleep");
  Serial.end();


  flashTransport.begin();
  flashTransport.runCommand(0xB9);
  flashTransport.end();

  
  sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  __WFE();
  __WFI();
  sd_app_evt_wait() ; //SOFT DEVICE “SYSTEM ON” POWER SAVE
  

  //disconnect
  nrf_gpio_cfg_default(g_ADigitalPinMap[11]);

  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[2], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH); //wake from deep sleep
  //delay(1000);
  NRF_POWER->SYSTEMOFF = 1;
  //nrf_pwr_mgmt_shutdown();

  //sd_power_system_off();
}
