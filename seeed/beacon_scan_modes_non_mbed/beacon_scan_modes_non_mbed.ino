
// Use function references:
// https://docs.nordicsemi.com/bundle/s130_v1.0.0_api/page/structble_gap_evt_adv_report_t_3.html#a389279e4505e5f438e39b64cac8ab4c0
// https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/blebeacon
#include <bluefruit.h>
#include <Wire.h>

#define MANUFACTURER_ID   0x0059
#define SSR_BEACON_ID     0x3F00 //Add ssr_id to it. Is a 8-bit value.

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
void init_beacon();             //Initialize the beacon BLE.
void set_data_beacon();         //Set the data received by I2C in the UUID and major, minor.
void start_beacon(uint16_t timer);  //Start the actual beacon and let it stop at a certain time defined in seconds.

void init_scan();
void start_scan(uint16_t timer);
void scan_callback(ble_gap_evt_adv_report_t* report);

void stop_callback();
void connect_callback(uint16_t conn_handle);

void receiveEvent(int howMany); //I2C callback
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

volatile struct ble_module_data_t i2c_data;
volatile struct ble_beacon_result_t beacon_data;

volatile bool received_data = false;

volatile bool mode = 0; //Default in beacon mode

long timer = 0;

void setup() 
{
  Serial.begin(115200);

  Wire.begin(0x12);             // join i2c bus with address 0x12
  Wire.onReceive(receiveEvent); // register event

  // Uncomment to blocking wait for Serial connection
  while ( !Serial ) delay(10);

  Serial.println("Wait for data");
  while(!received_data); //Wait for I2C transfer
  Serial.println("data received!");
  received_data = false;

  //Print the whole struct
  Serial.print("m: ");
  Serial.print(i2c_data.mode);
  Serial.print("id: ");
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
    Serial.printf("Amount of ACK's: %d", beacon_data.amount_of_ack);
    delay(200);
  
    deep_sleep();
  }
  else
  {
    //Mode 1

  }

}

void loop() 
{
  // loop is already suspended, CPU will not run loop() at all
}

void init_scan()
{
  Bluefruit.begin(0, 2);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName((const char*)(SSR_BEACON_ID |i2c_data.ssr_id));
}

void start_scan(uint16_t timer)
{
  // Start Central Scan
  Bluefruit.setConnLedInterval(250);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);

  uint8_t timer_sec = timer/1000;

  if (timer_sec <= 0)
    Bluefruit.Scanner.start(1);
  else
    Bluefruit.Scanner.start(timer_sec);

  Serial.println("setup - Scanning ...");
}

void init_beacon()
{
  Bluefruit.begin();
  Bluefruit.autoConnLed(true);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  beacon.setManufacturer(MANUFACTURER_ID);
  beacon.setMajorMinor((SSR_BEACON_ID |i2c_data.ssr_id), 0x0000);
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
  if(i2c_data.mode == 0){
    beacon_data.amount_of_ack++;
  }
  else{
    //Do somthing with data
    Serial.println("mode 1");
  }

  BLEConnection* conn = Bluefruit.Connection(conn_handle);
  Serial.println("connect_callback - Connected");
  conn->disconnect();
  Serial.println("connect_callback - The connection is disconnected");
}

void stop_callback()
{
  Serial.println("timer passed from .start() method Advertising");
  Serial.println("stopped");
  //deep_sleep();
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  //Serial.println("Entered scan");
  //The ID to identify 
  uint32_t id = report->data.p_data[report->data.len - 5] << 8*3 | 
                report->data.p_data[report->data.len - 4] << 8*2 |
                report->data.p_data[report->data.len - 3] << 8*1 | 
                report->data.p_data[report->data.len - 2] << 8*0;

  if (id == 1058013184 && id != devId)
  {
    devId = id;
    Serial.println("Timestamp Addr              Rssi Data");

    Serial.printf("%09d ", millis());
    
    // MAC is in little endian --> print reverse
    Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
    Serial.print(" ");
    Serial.print(*report->peer_addr.addr);
    Serial.print(" ");
    //Search for CE:A5:C5:43:A3:F2 242

    Serial.print(report->rssi);
    Serial.print("  ");

    Serial.printBuffer(report->data.p_data, report->data.len, '-');
    Serial.println();
                  
    Serial.printf("id: %02X  real val: %d", id, id);
    // Check if beacon has a certain address. Not good to use in global environment
    if ( *report->peer_addr.addr == 242 )//Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
    {
      Serial.println("                       BLE Beacon service detected");
      Serial.printf("%d ",report->data.len);
      Serial.printf(" id: %02X", id);
      uint8_t* pointer_to_data = report->data.p_data;
      uint16_t data_length = report->data.len;
      
      for( uint16_t i = 0; i < report->data.len; i++){
        
        Serial.print(i);
        Serial.print(" ");
        Serial.printf("%02X", pointer_to_data[0]);  
        //Serial.printf("%02X", pointer_to_data[1]);  
        //Serial.printf("%02X", pointer_to_data[2]);  
        //Serial.printf("%02X", pointer_to_data[3]);  
        Serial.print(" ; ");
        pointer_to_data += 1;
        
      }
      Serial.println("All data reviewed.");

      //Connect to the beacon
      //Will fail but is to provide an ACK to the beacon

      Serial.println("Before connect");
      Bluefruit.Central.connect(report);
      Serial.println("After connect");
      delay(500);
    }

    Serial.println();
  }
  else
  {
    Bluefruit.Scanner.resume();
  }

  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
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
