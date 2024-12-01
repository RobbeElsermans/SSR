#include <bluefruit.h>

#define BEACON_SSR_ID 0x3F

// IBeacon format used
// https://semiwiki.com/semiconductor-services/einfochips/302892-understanding-ble-beacons-and-their-applications/

#define TEMP_MSB 9
#define TEMP_LSB 10
#define HUM 11
#define LUX_MSB 12
#define LUX_LSB 13
#define VCC_MSB 14
#define VCC_LSB 15
#define RESERVED_1 16
#define RESERVED_1 17
#define RESERVED_1 18
#define RESERVED_1 19
#define RESERVED_1 20
#define RESERVED_1 21
#define RESERVED_1 22
#define RESERVED_1 23
#define RESERVED_1 24
#define MAJOR_MSB 25
#define MAJOR_LSB 26
#define MINOR_MSB 27
#define MINOR_LSB 28
#define RSSI 30


long timer = 0;

// uint8_t devId[256] = {0};
// uint8_t* dev_id_pointer = devId;

struct ble_beacon_receive_data_t
{
    uint8_t ssr_id;       // The ID of the source
    uint16_t temperature; // temperature
    uint8_t humidity;     // humidity
    uint16_t lux;         // lux (light)
    uint16_t voltage;     // voltage
    int8_t rssi;           //rssi
};

struct ble_beacon_receive_data_t received_beacon_data[256];
int16_t index_beacon_data = -1;

//--------------functions-------------//
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
void connect_callback(uint16_t conn_handle);
void scan_callback(ble_gap_evt_adv_report_t *report);
uint8_t fetch_id(ble_gap_evt_adv_report_t *report);
uint8_t fetch_ssr_id(ble_gap_evt_adv_report_t *report);
uint8_t ssr_id_exists(uint8_t ssr_id_unknown);
void increase_beacon_data_index();
void set_beacon_data(ble_gap_evt_adv_report_t *report);
void init_scan();
void start_scan(uint16_t timer);
void stop_callback();

//--------------functions-------------//

void setup()
{
    Serial.begin(115200);
    //while (!Serial)
    //    delay(10); // for nrf52840 with native usb

    Serial.println("Bluefruit52 Central Scan Example");
    Serial.println("--------------------------------\n");

    init_scan();

    uint8_t air_time = 200;

    //check if scanning timer has finished
    timer = millis();
    while(millis()-timer < air_time*100)
    {
      uint16_t time_left = air_time*100 - (millis()-timer);
      Serial.printf("time left: %d", time_left);
      
      start_scan(time_left);

      //perform scanning
      while(Bluefruit.Scanner.isRunning());
      
    }
    Serial.println("Finished air_time");
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

    // Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
    // Bluefruit.Scanner.restartOnDisconnect(false);   //Stops when a beacon is found and an ACK is sended over
    Bluefruit.Scanner.restartOnDisconnect(false);
    //Bluefruit.Scanner.start(0); // 500 is 5 seconds
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
void stop_callback()
{
  Serial.println("stop_callback - stopped");
}

void connect_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);
    Serial.println("connect_callback - A connection is made");
    conn->disconnect(); // in order to drop the connection
    Serial.println("connect_callback - The connection is disconnected");
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

void loop()
{
    // nothing to do
}
