#include <bluefruit.h>

void setup() 
{
  Serial.begin(115200);
  //while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 Central Scan Example");
  Serial.println("--------------------------------\n");

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // Start Central Scan
  Bluefruit.setConnLedInterval(250);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);

  Serial.println("Scanning ...");
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  //The ID to identify 
  uint32_t id = report->data.p_data[report->data.len - 5] << 8*3 | 
                report->data.p_data[report->data.len - 4] << 8*2 |
                report->data.p_data[report->data.len - 3] << 8*1 | 
                report->data.p_data[report->data.len - 2] << 8*0;

  if (id == 1058013184)
  {
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

//Compose 2 bytes of the 4 last bytes minus 1 byte
  uint32_t id = report->data.p_data[report->data.len - 5] << 8*3 | 
                report->data.p_data[report->data.len - 4] << 8*2 |
                report->data.p_data[report->data.len - 3] << 8*1 | 
                report->data.p_data[report->data.len - 2] << 8*0;
                
Serial.printf("id: %02X  real val: %d", id, id);
    // Check if beacon has a certain address. Not good to use in global environment
  if ( *report->peer_addr.addr == 242 )//Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
  {
    Serial.println("                       BLE UART service detected");
    Serial.println(report->data.len);
    //Serial.printf("id: %02X", id);
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
  }

  Serial.println();
  }

  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

void loop() 
{
  // nothing to do
}
