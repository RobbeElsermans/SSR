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
#include <bluefruit.h>
#include <Wire.h>

// Beacon uses the Manufacturer Specific Data field in the advertising
// packet, which means you must provide a valid Manufacturer ID. Update
// the field below to an appropriate value. For a list of valid IDs see:
// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
// 0x004C is Apple
// 0x0822 is Adafruit
// 0x0059 is Nordic
#define MANUFACTURER_ID   0x0059
#define SSR_BEACON_ID     0x3F00 //Add ssr_id to it. Is a 8-bit value.

// "nRF Connect" app can be used to detect beacon
uint8_t beaconUuid[16] =
{
  0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
  0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
//https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
BLEBeacon beacon(beaconUuid, 0x0000, 0x0000, -54);

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

volatile struct ble_module_data_t data;

void receiveEvent(int howMany);

volatile bool received_data = false;

void setup() 
{
  Serial.begin(115200);
  Wire.begin(0x12);             // join i2c bus with address 0x10
  Wire.onReceive(receiveEvent); // register event

  // Uncomment to blocking wait for Serial connection
  while ( !Serial ) delay(10);

  Serial.println("Bluefruit52 Beacon Example");
  Serial.println("--------------------------\n");

  Bluefruit.begin();

  // off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  
  beaconUuid[15] = random(255);
  
  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);

  Serial.println("Wait for data");
  while(!received_data); //Wait for I2C transfer
  Serial.println("data received!");
  received_data = false;

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

  beacon.setMajorMinor((SSR_BEACON_ID | data.ssr_id), 0x0000);

  beaconUuid[0] = (data.env_temperature & 0xFF) >> 8;
  beaconUuid[1] = (data.env_temperature & 0xFF);
  
  beaconUuid[2] = data.env_humidity;
  beaconUuid[3] = (data.env_lux & 0xFF) >> 8;
  beaconUuid[4] = (data.env_lux & 0xFF);
  beaconUuid[5] = (data.dev_voltage & 0xFF) >> 8;
  beaconUuid[6] = (data.dev_voltage & 0xFF);
  
  // Setup the advertising packet
  startAdv();

  Serial.println("Broadcasting beacon, open your beacon app to test");

  //beaconUuid[15] = 0xFF;

  while(Bluefruit.Advertising.isRunning());

  //delay(10000);
  Serial.println("Done Advertising");

  Serial.println("Go to deep sleep");
  //https://pperego83.medium.com/how-to-sleep-arduino-nano-ble-33-c4fe4d38b357
  //https://forum.arduino.cc/t/put-nano-33-ble-into-power-on-sleep-mode/1169463
  //https://forum.seeedstudio.com/t/sleep-current-of-xiao-nrf52840-deep-sleep-vs-light-sleep/271841
  //https://devzone.nordicsemi.com/f/nordic-q-a/100602/seeed-xiao-nrf82840-wakeup-after-nrf_power-systemoff

  nrf_gpio_cfg_sense_input(digitalPinToInterrupt(2), NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
  delay(2000);

  NRF_POWER->SYSTEMOFF = 1;
  //Not working
  //Continue here -> https://devzone.nordicsemi.com/f/nordic-q-a/58631/waking-the-nrf52840-from-systemoff---arduino-nano-33-board

  // Suspend Loop() to save power, since we didn't have any code there
  //suspendLoop();
  Serial.println("OK");
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
}

void startAdv(void)
{  
  // Advertising packet
  // Set the beacon payload using the BLEBeacon class populated
  // earlier in this example
  Bluefruit.Advertising.setBeacon(beacon);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * Apple Beacon specs
   * - Type: Non connectable, undirected
   * - Fixed interval: 100 ms -> fast = slow = 100 ms
   */
  //Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  //Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(5);                // 0 = Don't stop advertising after n seconds  
}

void loop() 
{
  // loop is already suspended, CPU will not run loop() at all
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    uint8_t i = 0;
    while (Wire.available()) // loop through all but the last
    {
        uint8_t c = Wire.read(); // receive byte as a character

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
            data.env_temperature = (int16_t)(c << 8 | c1);
        }

        if (i == 5) // env_humidity
            data.env_humidity = (uint8_t)c;
        if (i == 6) // env_lux
        {
            i++;
            
            uint8_t c1 = Wire.read();
            data.env_lux = (int16_t)(c << 8 | c1);
        }

        if (i == 8) // dev_voltage
        {   
            i++;
            uint8_t c1 = Wire.read();
            data.dev_voltage = (uint16_t) (c << 8 | c1);
        }

        i++;
    }
    received_data = true;
}
