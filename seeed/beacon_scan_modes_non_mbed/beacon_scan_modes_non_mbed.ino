
// Use function references:
// https://docs.nordicsemi.com/bundle/s130_v1.0.0_api/page/structble_gap_evt_adv_report_t_3.html#a389279e4505e5f438e39b64cac8ab4c0
// https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
#include <bluefruit.h>
#include <Wire.h>
#include <Adafruit_SPIFlash.h>

//#define DEBUG
#define RED_LED 11

#define MANUFACTURER_ID 0x0059 // Defines that it is Nordic semiconductors manufacturer

#define BEACON_SSR_ID 0x3F  // Add ssr_id to it. Is a 8-bit value.
#define BLE_IC_ADDRESS 0x12 // The I2C address

Adafruit_FlashTransport_QSPI flashTransport; // object to access flash.

// IBeacon format used
// https://semiwiki.com/semiconductor-services/einfochips/302892-understanding-ble-beacons-and-their-applications/

#define TEMP_MSB 9
#define TEMP_LSB 10
#define HUM 11
#define LUX_MSB 12
#define LUX_LSB 13
#define VCC_MSB 14
#define VCC_LSB 15
#define GYRO_X 16
#define GYRO_Y 17
#define GYRO_Z 18
// #define RESERVED_1 19
// #define RESERVED_1 20
// #define RESERVED_1 21
// #define RESERVED_1 22
// #define RESERVED_1 23
// #define RESERVED_1 24
#define MAJOR_MSB 25
#define MAJOR_LSB 26
#define MINOR_MSB 27
#define MINOR_LSB 28
#define RSSI 30

#define TIMEOUT_RECEIVE_WINDOW 4000

// "nRF Connect" app can be used to detect beacon
uint8_t beaconUuid[16] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
// https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
// We will set the major and minor further down in the code.
BLEBeacon beacon(beaconUuid, 0x0000, 0x0000, -54);

void deep_sleep(); // Deep sleep configuration with HIGH-flank wakeup on pin D2.

//--------------beacon functions-------------//
void init_beacon();                // Initialize the beacon BLE.
void set_data_beacon();            // Set the data received by I2C in the UUID and major, minor.
void start_beacon(uint16_t timer); // Start the actual beacon and let it stop at a certain time defined in seconds.

//--------------scanner functions-------------//
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
void scan_callback(ble_gap_evt_adv_report_t *report);
uint8_t fetch_id(ble_gap_evt_adv_report_t *report);
uint8_t fetch_ssr_id(ble_gap_evt_adv_report_t *report);
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
void increase_beacon_data_index();
void set_beacon_data(ble_gap_evt_adv_report_t *report);
void init_scan();
void start_scan(uint16_t timer);
// void stop_callback();
void stop_callback();
void connect_callback(uint16_t conn_handle);

//--------------I2C callback-------------//
void receive_event(int howMany);
void request_event();
uint32_t devId = 0;

//--------------BLE struct-------------//
struct ble_module_data_t
{
  uint8_t mode;            // The mode of the BLE-module, 0 -> beacon, 1-> scan
  uint8_t ssr_id;          // The ID of the rover itself
  uint8_t air_time;        // How long the beacon may last (val*100=ms)
  int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
  uint8_t env_humidity;    // Range from -0-100%
  uint16_t env_lux;        // Range from 0 to 1000
  uint16_t dev_voltage;    // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  int8_t dev_gyro_x;       // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_y;       // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_z;       // Range from -60 to 60 (val*3=°)
};

//--------------BLE Beacon result struct-------------//
struct ble_beacon_result_t
{
  // How many devices have acked the Beacon. this determines the amount of nodes present
  uint8_t amount_of_ack;
};

//--------------BLE Scan result struct-------------//
struct ble_scan_data_t
{
  uint8_t ssr_id;          // The ID of the source
  int16_t env_temperature; // temperature
  uint8_t env_humidity;    // humidity
  uint16_t env_lux;        // lux (light)
  uint16_t dev_voltage;    // voltage
  int8_t dev_gyro_x;       // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_y;       // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_z;       // Range from -60 to 60 (val*3=°)
  int8_t rssi;             // rssi
};

volatile struct ble_module_data_t i2c_data;
volatile struct ble_beacon_result_t beacon_data;
volatile struct ble_scan_data_t received_beacon_data[2];
volatile int16_t index_beacon_data = -1;

volatile bool received_data = false;   // Valid data received over I2C
volatile bool device_is_done = false;  // BLE device has finished its process
volatile bool transmited_data = false; // Master has pulled the local data

volatile bool mode = 0; // Default in beacon mode

long timer = 0; // Local timer to set a certain duration of the process

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
    // Uncomment to blocking wait for Serial connection
  //while ( !Serial ) delay(10);
  //delay(500);
#endif
  Wire.begin(0x12);              // join i2c bus with address 0x12
  Wire.onReceive(receive_event); // register event
  Wire.onRequest(request_event);

  pinMode(RED_LED, OUTPUT);

#ifdef DEBUG
  Serial.println("Wait for data");
#endif
  int tryouts = 40; // wait a maximum of 4 seconds before continuing
  while (!received_data)
  {
    delay(100); // Wait for I2C transfer

    //Check when the maximum is reached
    tryouts--;
    if(tryouts == 0)
      deep_sleep();
  }

#ifdef DEBUG
  Serial.println("data received! ");
#endif

  received_data = false;
  uint16_t time_left = 0;

  if (i2c_data.mode == 0)
  {
    init_beacon();
    set_data_beacon();
    digitalWrite(RED_LED, LOW);
    timer = millis();
    while (millis() - timer < i2c_data.air_time * 100)
    {
      time_left = i2c_data.air_time * 100 - (millis() - timer);
#ifdef DEBUG
      Serial.printf("time left: %d", time_left);
#endif

      start_beacon(time_left); // Pass through the amount of seconds it has to wait.

      // Wait until a connection is made again
      while (Bluefruit.Advertising.isRunning())
      {
        delay(100); // Give the PMU time to optimize current consumption
      }

#ifdef DEBUG
      Serial.println("advertising done");
#endif

      Bluefruit.Periph.clearBonds();

      // Continue the loop for beacon
    }

#ifdef DEBUG
    Serial.printf("Amount of ACK's: %d \r\n", beacon_data.amount_of_ack);
#endif
  }
  else
  {
    init_scan();
    digitalWrite(RED_LED, LOW);
    timer = millis();
    while (millis() - timer < i2c_data.air_time * 100)
    {
      time_left = i2c_data.air_time * 100 - (millis() - timer);

#ifdef DEBUG
      Serial.printf("time left: %d", time_left);
#endif

      start_scan(time_left);

      // perform scanning
      while (Bluefruit.Scanner.isRunning())
      {
        delay(100); // Give the PMU time to optimize current consumption
      }

      // Stop the loop for scanner
      //  request_event must be altered if more beacons can be scanned.
      break;
    }
  }

  digitalWrite(RED_LED, HIGH);

  time_left = i2c_data.air_time * 100 - (millis() - timer);
  // Wait for I2C read from master
  if (time_left > 0)
    time_left += TIMEOUT_RECEIVE_WINDOW; // Make sure the STM32 waits evenly

#ifdef DEBUG
  Serial.printf("time left 2: %d", time_left);
#endif

  device_is_done = true;

  timer = millis();
  while (!transmited_data && (millis() - timer) < time_left)
  {
    delay(100);
  }

  deep_sleep(); // Go to sleep if no one asked for the data.
}

void loop()
{
  // loop is already suspended, CPU will not run loop() at all
}

void init_beacon()
{
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(2); // Check bluefruit.h for supported values
  beacon.setManufacturer(MANUFACTURER_ID);
  beacon.setMajorMinor((BEACON_SSR_ID << 8 | i2c_data.ssr_id), 0x0000);
}

void set_data_beacon()
{
  beaconUuid[0] = (i2c_data.env_temperature >> 8) & 0xFF;
  beaconUuid[1] = (i2c_data.env_temperature & 0xFF);
  beaconUuid[2] = i2c_data.env_humidity;
  beaconUuid[3] = (i2c_data.env_lux >> 8) & 0xFF;
  beaconUuid[4] = (i2c_data.env_lux & 0xFF);
  beaconUuid[5] = (i2c_data.dev_voltage >> 8) & 0xFF;
  beaconUuid[6] = (i2c_data.dev_voltage & 0xFF);
  beaconUuid[7] = (i2c_data.dev_gyro_x);
  beaconUuid[8] = (i2c_data.dev_gyro_y);
  beaconUuid[9] = (i2c_data.dev_gyro_z);
}

void start_beacon(uint16_t timer)
{
  // Advertising packet
  // Set the beacon payload using the BLEBeacon class populated earlier in this code (set_data_beacon)
  Bluefruit.Advertising.setBeacon(beacon);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Advertising.setStopCallback(stop_callback);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  Bluefruit.Advertising.restartOnDisconnect(false);
  Bluefruit.Advertising.setInterval(80, 80); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(1);   // number of seconds in fast mode

  uint8_t timer_sec = timer / 1000; // Convert from milliseconds to seconds

#ifdef DEBUG
  Serial.println("Time in sec");
  Serial.println(timer_sec);
#endif

  if (timer_sec <= 0)
  {
    Bluefruit.Advertising.start(1); // number of seconds in fast mode
  }
  else
  {
    Bluefruit.Advertising.start(timer_sec);
  }
}

void connect_callback(uint16_t conn_handle)
{
  BLEConnection *conn = Bluefruit.Connection(conn_handle);
  conn->disconnect();

  if (i2c_data.mode == 0) // Beacon Mode
  {
    beacon_data.amount_of_ack++;
    Bluefruit.Advertising.stop(); // Stop advertiser
  }
  else // Scan mode
  {
    Bluefruit.Scanner.stop(); // Stop scanner
  }
}

void stop_callback()
{
#ifdef DEBUG
  Serial.println("stop_callback - stopped");
#endif
}

void init_scan()
{
  Bluefruit.begin(0, 2);   // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 2
  Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
  Bluefruit.setName((const char *)("I2C SSR data"));
  Bluefruit.autoConnLed(false);                           // Disable led
  Bluefruit.Scanner.setRxCallback(scan_callback);         // Scan callback
  Bluefruit.Central.setConnectCallback(connect_callback); // Connection callback
  // Bluefruit.Scanner.setStopCallback(stop_callback);     //Stop callback
  Bluefruit.Scanner.restartOnDisconnect(false); // Do not restart auto after disconnect
}

void start_scan(uint16_t timer)
{
  uint8_t timer_sec = timer / 1000;

#ifdef DEBUG
  Serial.printf("start_scan - timer_sec: %d\r\n", timer_sec);
#endif

  if (timer_sec <= 0)
    Bluefruit.Scanner.start(1 * 100);
  else
    Bluefruit.Scanner.start(timer_sec * 100);

#ifdef DEBUG
  Serial.println("start_scan - start scan ...");
#endif
}

void scan_callback(ble_gap_evt_adv_report_t *report)
{
  uint8_t beacon_id = fetch_id(report);  // The BEACON ID to identify
  uint8_t ssr_id = fetch_ssr_id(report); // The SSR ID, is unique

  // Check if correct beacon ID is found and the SSR_ID is not yet scanned.
  if (beacon_id == BEACON_SSR_ID && !ssr_id_exists(ssr_id))
  {
    set_beacon_data(report); // Add beacon data

#ifdef DEBUG
    Serial.println(" ----- BLE SSR Beacon service detected ----- ");
    Serial.printf(" Rssi: %d ", report->rssi);
    Serial.printf("bid: 0x%02X ssrid: 0x%02X", beacon_id, ssr_id);
    Serial.printBuffer(report->data.p_data, report->data.len, '-');
    Serial.println();
    Serial.printf("bid: 0x%02X ssrid: 0x%02X \r\n", beacon_id, ssr_id);
    Serial.printf("temp: %d \r\n", received_beacon_data[index_beacon_data].env_temperature);
    Serial.printf("hum: %d \r\n", received_beacon_data[index_beacon_data].env_humidity);
    Serial.printf("lux: %d \r\n", received_beacon_data[index_beacon_data].env_lux);
    Serial.printf("voltage: %d \r\n", received_beacon_data[index_beacon_data].dev_voltage);
    Serial.printf("gx: %d \r\n", received_beacon_data[index_beacon_data].dev_gyro_x);
    Serial.printf("gy: %d \r\n", received_beacon_data[index_beacon_data].dev_gyro_y);
    Serial.printf("gz: %d \r\n", received_beacon_data[index_beacon_data].dev_gyro_z);
    Serial.printf("rssi: %d \r\n", received_beacon_data[index_beacon_data].rssi);
    Serial.println("All data reviewed.");
    Serial.println();
#endif

    // Connect to the beacon
    Bluefruit.Central.connect(report); // Connect to beacon to let it known someone has found him.
  }
  else
  {
    Bluefruit.Scanner.resume(); // If the wrong beacon is found or the SSR_ID already exists, continue with scanning
  }
}

uint8_t fetch_id(ble_gap_evt_adv_report_t *report)
{
  return report->data.p_data[MAJOR_MSB];
}

uint8_t fetch_ssr_id(ble_gap_evt_adv_report_t *report)
{
  return report->data.p_data[MAJOR_LSB];
}

uint8_t ssr_id_exists(uint8_t ssr_id_unknown)
{
  for (uint8_t i = 0; i < 256; i++)
  {
    if (received_beacon_data[i].ssr_id == ssr_id_unknown)
    {
      return 1;
    }

    if (received_beacon_data[i].ssr_id == 0)
      return 0;
  }
  return 0;
}

void increase_beacon_data_index()
{
  if (index_beacon_data == 255)
    index_beacon_data = 255;
  else
    index_beacon_data++;

#ifdef DEBUG
  Serial.printf("\r\n increase_beacon_data_index to %d \r\n", index_beacon_data);
#endif
}

void set_beacon_data(ble_gap_evt_adv_report_t *report)
{
  increase_beacon_data_index(); // Increase the local index of stored beacons in the area.

  received_beacon_data[index_beacon_data].ssr_id = fetch_ssr_id(report);
  received_beacon_data[index_beacon_data].env_temperature = report->data.p_data[TEMP_MSB] << 8 | report->data.p_data[TEMP_LSB];
  received_beacon_data[index_beacon_data].env_humidity = report->data.p_data[HUM];
  received_beacon_data[index_beacon_data].env_lux = report->data.p_data[LUX_MSB] << 8 | report->data.p_data[LUX_LSB];
  received_beacon_data[index_beacon_data].dev_voltage = report->data.p_data[VCC_MSB] << 8 | report->data.p_data[VCC_LSB];
  received_beacon_data[index_beacon_data].dev_gyro_x = report->data.p_data[GYRO_X];
  received_beacon_data[index_beacon_data].dev_gyro_y = report->data.p_data[GYRO_Y];
  received_beacon_data[index_beacon_data].dev_gyro_z = report->data.p_data[GYRO_Z];
  received_beacon_data[index_beacon_data].rssi = report->rssi;
}

void request_event()
{
  if (device_is_done)
  {
    transmited_data = true; // Someone has requested data

    if (i2c_data.mode == 1) // Scan mode
    {
      if (received_beacon_data[0].ssr_id != 0x00)
      {
        // For now, we only transmit the first beacon found.
        // Later optimize this so we can transmit multiple beacons that are found

        Wire.write(received_beacon_data[0].ssr_id);
        Wire.write((received_beacon_data[0].env_temperature >> 8) & 0xFF);
        Wire.write(received_beacon_data[0].env_temperature & 0xFF);
        Wire.write(received_beacon_data[0].env_humidity);
        Wire.write((received_beacon_data[0].env_lux >> 8) & 0xFF);
        Wire.write(received_beacon_data[0].env_lux & 0xFF);
        Wire.write((received_beacon_data[0].dev_voltage >> 8) & 0xFF);
        Wire.write(received_beacon_data[0].dev_voltage & 0xFF);
        Wire.write(received_beacon_data[0].dev_gyro_x);
        Wire.write(received_beacon_data[0].dev_gyro_y);
        Wire.write(received_beacon_data[0].dev_gyro_z);
        Wire.write(received_beacon_data[0].rssi);

        Wire.write(255 - received_beacon_data[0].ssr_id); // Create a failsafe value to make sure the sended value is correct.

#ifdef DEBUG
        Serial.printf("write mode 1: %d\r\n", received_beacon_data[0].ssr_id);
#endif
      }
      else // No beacon found mode
      {
        for (uint8_t i = 0; i < 12; i++)
        {
          Wire.write(0);
        }
        Wire.write(0); // Add this so that the master knows no one is detected.
      }
    }
    else // in beacon mode
    {
      Wire.write(beacon_data.amount_of_ack);
      Wire.write(255 - beacon_data.amount_of_ack); // Create a failsafe value to make sure the sended value is correct.

#ifdef DEBUG
      Serial.printf("write mode 0: %d\r\n", beacon_data.amount_of_ack);
#endif
    }

#ifdef DEBUG
    Serial.printf("A request on I2C\r\n");
#endif

    // Data is readed so go to sleep.
    //deep_sleep();
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receive_event(int howMany)
{
  uint8_t i = 0;

#ifdef DEBUG
  Serial.printf("size available: %d, hommany: %d", Wire.available(), howMany);
#endif

  if (howMany == 13) // To make sure we recieve the correct amount of data
  {
    while (Wire.available()) // loop through all but the last
    {
      uint8_t c = Wire.read(); // receive byte as a character
      if (i == 0)
        i2c_data.mode = (uint8_t)c;
      if (i == 1) // ssr_id
        i2c_data.ssr_id = (uint8_t)c;
      if (i == 2) // beacon_time
        i2c_data.air_time = (uint8_t)c;
      if (i == 3) // env_temperature HSB
      {
        i++;
        uint8_t c1 = Wire.read();
        i2c_data.env_temperature = (int16_t)(c << 8 | c1);
      }

      if (i == 5) // env_humidity
        i2c_data.env_humidity = (uint8_t)c;
      if (i == 6) // env_lux
      {
        i++;
        uint8_t c1 = Wire.read();
        i2c_data.env_lux = (int16_t)(c << 8 | c1);
      }

      if (i == 8) // dev_voltage
      {
        i++;
        uint8_t c1 = Wire.read();
        i2c_data.dev_voltage = (uint16_t)(c << 8 | c1);
      }

      if (i == 10) // dev_gyro_x
        i2c_data.dev_gyro_x = (uint16_t)c;

      if (i == 11) // dev_gyro_y
        i2c_data.dev_gyro_y = (uint16_t)c;

      if (i == 12) // dev_gyro_z
        i2c_data.dev_gyro_z = (uint16_t)c;

      i++;
    }
  }

  // If ID has a good value which is not equal to 0
  if (i2c_data.ssr_id != 0x00)
    received_data = true;

#ifdef DEBUG
  if (received_data)
  {
    // Print the whole struct
    Serial.print("m: ");
    Serial.print(i2c_data.mode);
    Serial.print(" id: ");
    Serial.print(i2c_data.ssr_id);
    Serial.print(" at: ");
    Serial.print(i2c_data.air_time);
    Serial.print(" et: ");
    Serial.print(i2c_data.env_temperature);
    Serial.print(" eh: ");
    Serial.print(i2c_data.env_humidity);
    Serial.print(" el: ");
    Serial.print(i2c_data.env_lux);
    Serial.print(" egx: ");
    Serial.print(i2c_data.dev_gyro_y);
    Serial.print(" egy: ");
    Serial.print(i2c_data.dev_gyro_z);
    Serial.print(" egz: ");
    Serial.print(i2c_data.dev_gyro_x);
    Serial.print(" dv: ");
    Serial.println(i2c_data.dev_voltage);
  }
#endif
}

void deep_sleep()
{
#ifdef DEBUG
  Serial.println("Go to deep sleep");
  delay(1000);
  Serial.end();
#endif

  Wire.end();

  // Turn of flash, has like 15µA consumption.
  flashTransport.begin();
  flashTransport.runCommand(0xB9);
  flashTransport.end();

  // wake pin from deep sleep
  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[2], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

  // set deep sleep power off mode.
  NRF_POWER->SYSTEMOFF = 1;
}
