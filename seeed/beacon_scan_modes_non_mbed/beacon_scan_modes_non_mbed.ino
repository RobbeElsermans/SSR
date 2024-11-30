
// Use function references:
// https://docs.nordicsemi.com/bundle/s130_v1.0.0_api/page/structble_gap_evt_adv_report_t_3.html#a389279e4505e5f438e39b64cac8ab4c0
// https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
#include <bluefruit.h>
#include <Wire.h>

#define MANUFACTURER_ID   0x0059

#define BEACON_SSR_ID 0x3F  //Add ssr_id to it. Is a 8-bit value.

// IBeacon format used
// https://semiwiki.com/semiconductor-services/einfochips/302892-understanding-ble-beacons-and-their-applications/

#define TEMP_MSB 9
#define TEMP_LSB 10
#define HUM 11
#define LUX_MSB 12
#define LUX_LSB 13
#define VCC_MSB 14
#define VCC_LSB 15
//#define RESERVED_1 16
//#define RESERVED_1 17
//#define RESERVED_1 18
//#define RESERVED_1 19
//#define RESERVED_1 20
//#define RESERVED_1 21
//#define RESERVED_1 22
//#define RESERVED_1 23
//#define RESERVED_1 24
#define MAJOR_MSB 25
#define MAJOR_LSB 26
#define MINOR_MSB 27
#define MINOR_LSB 28
#define RSSI 30

// "nRF Connect" app can be used to detect beacon
uint8_t beaconUuid[16] =
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
//https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
//We will set the major and minor further down in the code.
BLEBeacon beacon(beaconUuid, 0x0000, 0x0000, -54);

void deep_sleep();              //Deep sleep configuration with HIGH-flank wakeup on pin D2.

//--------------beacon functions-------------//
void init_beacon();             //Initialize the beacon BLE.
void set_data_beacon();         //Set the data received by I2C in the UUID and major, minor.
void start_beacon(uint16_t timer);  //Start the actual beacon and let it stop at a certain time defined in seconds.


//--------------scanner functions-------------//
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
//void connect_callback(uint16_t conn_handle);
void scan_callback(ble_gap_evt_adv_report_t *report);
uint8_t fetch_id(ble_gap_evt_adv_report_t *report);
uint8_t fetch_ssr_id(ble_gap_evt_adv_report_t *report);
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
void increase_beacon_data_index();
void set_beacon_data(ble_gap_evt_adv_report_t *report);
void init_scan();
void start_scan(uint16_t timer);
//void stop_callback();
void stop_callback();
void connect_callback(uint16_t conn_handle);

void receive_event(int howMany); //I2C callback
void request_event();
uint32_t devId = 0;

struct ble_module_data_t
{
    uint8_t mode;             // The mode of the BLE-module, 0 -> beacon, 1-> scan
    uint8_t ssr_id;           // The ID of the rover itself
    uint8_t air_time; // How long the beacon may last (val*100=ms)
    int16_t env_temperature;  // Range from -327.68 to 327.67 °C (val/100=°C)
    uint8_t env_humidity;     // Range from -0-100%
    uint16_t env_lux;         // Range from 0 to 1000
    uint16_t dev_voltage;     // Range from 0-6.5535V (val/10000=V) (val/10=mV)
    // x gyro 8bit?
    // y gyro 8bit?
    // z gyro 8bit?
};

struct ble_beacon_result_t
{
  // How many devices have acked the Beacon. this determines the amount of nodes present
  uint8_t amount_of_ack;
};

struct ble_scan_data_t
{
    uint8_t ssr_id;       // The ID of the source
    uint16_t temperature; // temperature
    uint8_t humidity;     // humidity
    uint16_t lux;         // lux (light)
    uint16_t voltage;     // voltage
    int8_t rssi;           //rssi
};

volatile struct ble_module_data_t i2c_data;
volatile struct ble_beacon_result_t beacon_data;
volatile struct ble_scan_data_t received_beacon_data[256];
volatile int16_t index_beacon_data = -1;

volatile bool received_data = false;
volatile bool device_is_done = false;
volatile bool transmited_data = false;

volatile bool mode = 0; //Default in beacon mode

long timer = 0;

void setup() 
{
  Serial.begin(115200);

  Wire.begin(0x12);             // join i2c bus with address 0x12
  Wire.onReceive(receive_event); // register event
  Wire.onRequest(request_event);

  // Uncomment to blocking wait for Serial connection
  while ( !Serial ) delay(10);

  Serial.println("Wait for data");
  while(!received_data); //Wait for I2C transfer
  Serial.println("data received! ");
  received_data = false;

  if(i2c_data.mode == 0)
  {
    init_beacon();
    set_data_beacon();
    Serial.println("Broadcasting beacon, open your beacon app to test");
    timer = millis();
    while(millis()-timer < i2c_data.air_time*100)
    {
      uint16_t time_left = i2c_data.air_time*100 - (millis()-timer);
      //Pass through the amount of seconds it has to wait.
      Serial.printf("time left: %d", time_left);
      start_beacon(time_left);
      //Wait until a connection is made again
      while(Bluefruit.Advertising.isRunning());
      Serial.println("advertising done");
      delay(1);
      Bluefruit.Periph.clearBonds();
    }
  
    //Transfer data of beacon to master
    Serial.printf("Amount of ACK's: %d \r\n", beacon_data.amount_of_ack);
    delay(200);
  }
  else
  {
    init_scan();

    //check if scanning timer has finished
    timer = millis();
    while(millis()-timer < i2c_data.air_time*100)
    {
      uint16_t time_left = i2c_data.air_time*100 - (millis()-timer);
      Serial.printf("time left: %d", time_left);
      
      start_scan(time_left);

      //perform scanning
      while(Bluefruit.Scanner.isRunning());
      
    }

  }

  //Wait for I2C read from master
  device_is_done = true;
  while(!transmited_data)
  {
    delay(10);
  }
  
  deep_sleep();
}

void loop() 
{
  // loop is already suspended, CPU will not run loop() at all
}

void init_beacon()
{
  Bluefruit.begin();
  Bluefruit.autoConnLed(true);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  beacon.setManufacturer(MANUFACTURER_ID);
  beacon.setMajorMinor((BEACON_SSR_ID << 8 |i2c_data.ssr_id), 0x0000);
}

void set_data_beacon()
{
  beaconUuid[0] = (i2c_data.env_temperature & 0xFF) >> 8;
  beaconUuid[1] = (i2c_data.env_temperature & 0xFF);
  
  beaconUuid[2] = i2c_data.env_humidity;
  beaconUuid[3] = (i2c_data.env_lux & 0xFF) >> 8;
  beaconUuid[4] = (i2c_data.env_lux & 0xFF);
  beaconUuid[5] = (i2c_data.dev_voltage & 0xFF) >> 8;
  beaconUuid[6] = (i2c_data.dev_voltage & 0xFF);
}

void start_beacon(uint16_t timer)
{
  // Advertising packet
  // Set the beacon payload using the BLEBeacon class populated
  // earlier in this example
  Bluefruit.Advertising.setBeacon(beacon);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Advertising.setStopCallback(stop_callback);

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
//  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
    
  //Bluefruit.Advertising.restartOnDisconnect(false); //in order so the beacon stops sending after ACK.
  Bluefruit.Advertising.restartOnDisconnect(false);
  Bluefruit.Advertising.setInterval(160,160);    // in unit of 0.625 ms
  
  Bluefruit.Advertising.setFastTimeout(1);      // number of seconds in fast mode
  //Bluefruit.Advertising.start(data.air_time/10);                // 0 = Don't stop advertising after n seconds  

  uint8_t timer_sec = timer/1000;

  Serial.println("Time in sec");
  Serial.println(timer_sec);

  if (timer_sec <= 0)
    Bluefruit.Advertising.start(1);
  else
    Bluefruit.Advertising.start(timer_sec);
}

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void connect_callback(uint16_t conn_handle)
{
    BLEConnection* conn = Bluefruit.Connection(conn_handle);
    conn->disconnect();
  
  if(i2c_data.mode == 0){
    beacon_data.amount_of_ack++;
  }
  else{
    Bluefruit.Scanner.stop();
  }
}

void stop_callback()
{
  Serial.println("stop_callback - timer passed");
  Serial.println("stop_callback - stopped");
  //deep_sleep();
}


void init_scan(){
    // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
    // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 2);
    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.setName((const char*)("I2C SSR data"));
    
    // Start Central Scan
    Bluefruit.setConnLedInterval(250);
    Bluefruit.Scanner.setRxCallback(scan_callback);
    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Scanner.setStopCallback(stop_callback);
    Bluefruit.Scanner.restartOnDisconnect(false);

}

void start_scan(uint16_t timer){
  uint8_t timer_sec = timer/1000;
  Serial.printf("start_scan - timer_sec: %d\r\n", timer_sec);
  if (timer_sec <= 0)
    Bluefruit.Scanner.start(1*100);
  else
    Bluefruit.Scanner.start(timer_sec*100);

    Serial.println("start_scan - start scan ...");
}

void scan_callback(ble_gap_evt_adv_report_t *report)
{
    // The ID to identify
    uint8_t beacon_id = fetch_id(report);
    uint8_t ssr_id = fetch_ssr_id(report);

    if (beacon_id == BEACON_SSR_ID && !ssr_id_exists(ssr_id))
    {
        Serial.println(" ----- BLE SSR Beacon service detected ----- ");
        Serial.printf(" Rssi: %d ", report->rssi);
        Serial.printf("bid: 0x%02X ssrid: 0x%02X", beacon_id, ssr_id);

        set_beacon_data(report);

        Serial.printBuffer(report->data.p_data, report->data.len, '-');
        Serial.println();

        Serial.printf("bid: 0x%02X ssrid: 0x%02X \r\n", beacon_id, ssr_id);
        Serial.printf("temp: %d \r\n", received_beacon_data[index_beacon_data].temperature);
        Serial.printf("hum: %d \r\n", received_beacon_data[index_beacon_data].humidity);
        Serial.printf("lux: %d \r\n", received_beacon_data[index_beacon_data].lux);
        Serial.printf("voltage: %d \r\n", received_beacon_data[index_beacon_data].voltage);
        Serial.printf("rssi: %d \r\n", received_beacon_data[index_beacon_data].rssi);

        Serial.println("All data reviewed.");

        // Connect to the beacon
        Bluefruit.Central.connect(report);
        Serial.println();
    }
    else
    {
        Bluefruit.Scanner.resume();
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
    {
        index_beacon_data = 255;
    }
    else
    {
        index_beacon_data++;
    }

    Serial.printf("\r\n increase_beacon_data_index to %d \r\n", index_beacon_data);
}

void set_beacon_data(ble_gap_evt_adv_report_t *report)
{
    increase_beacon_data_index();

    received_beacon_data[index_beacon_data].ssr_id = fetch_ssr_id(report);
    received_beacon_data[index_beacon_data].temperature = report->data.p_data[TEMP_MSB] << 8 | report->data.p_data[TEMP_LSB];
    received_beacon_data[index_beacon_data].humidity = report->data.p_data[HUM];
    received_beacon_data[index_beacon_data].lux = report->data.p_data[LUX_MSB] << 8 | report->data.p_data[LUX_LSB];
    received_beacon_data[index_beacon_data].voltage = report->data.p_data[VCC_MSB] << 8 | report->data.p_data[VCC_LSB];
    received_beacon_data[index_beacon_data].rssi = report->rssi;
}

void request_event()
{
  if(device_is_done)
  {
    transmited_data = true;

    if(i2c_data.mode = 0){
      Wire.write(10);
    }
    else{
      Wire.write(beacon_data.amount_of_ack);
    }
    
    Serial.printf("A request on I2C: %d\r\n", Wire.read());
    //Wire.write(20); 
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receive_event(int howMany)
{
    uint8_t i = 0;
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
           i2c_data.dev_voltage = (uint16_t) (c << 8 | c1);
        }

        i++;
    }

    //If ID has a good value
    if (i2c_data.ssr_id != 0x00)
      received_data = true;


    if (received_data){
        //Print the whole struct
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
        Serial.print(" dv: ");
        Serial.println(i2c_data.dev_voltage);
    }
}

void deep_sleep(){
  Serial.println("Go to deep sleep");
  delay(1000);
  //https://pperego83.medium.com/how-to-sleep-arduino-nano-ble-33-c4fe4d38b357
  //https://forum.arduino.cc/t/put-nano-33-ble-into-power-on-sleep-mode/1169463
  //https://forum.seeedstudio.com/t/sleep-current-of-xiao-nrf52840-deep-sleep-vs-light-sleep/271841
  //https://devzone.nordicsemi.com/f/nordic-q-a/100602/seeed-xiao-nrf82840-wakeup-after-nrf_power-systemoff

  //nrf_gpio_cfg_sense_input(digitalPinToInterrupt(2), NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

  //Works
  //Found it on https://forum.seeedstudio.com/t/xiao-ble-sense-in-deep-sleep-mode/263477/129?page=7
  //nrf_gpio_cfg_sense_input(g_ADigitalPinMap[2], NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW); //wake from deep sleep
  nrf_gpio_cfg_sense_input(g_ADigitalPinMap[2], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH); //wake from deep sleep

  NRF_POWER->SYSTEMOFF = 1;
}
