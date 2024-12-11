#include "ble_module.h"
#include "usart.h"

delay_callback_t _delay_callback;

void bleDelayCallback(delay_callback_t dc_fp)
{
  _delay_callback = dc_fp;
}


ble_transmit_t _ble_send;
void bleSendCallback(ble_transmit_t ble_transmit_fp)
{
  _ble_send = ble_transmit_fp;
}


ble_receive_t _ble_receive;
void bleReceiveCallback(ble_receive_t ble_receive_fp)
{
  _ble_receive = ble_receive_fp;
}


ble_slave_available_t _ble_slave_available;
void bleAvailableCallback(ble_slave_available_t ble_available_fp)
{
  _ble_slave_available = ble_available_fp;
}


wake_device_t _wake_device;
void bleWakeCallback(wake_device_t wake_device_fp)
{
  _wake_device = wake_device_fp;
}

uint8_t send_ble_data(I2C_HandleTypeDef *hi2c1, ble_module_data_t *data)
{
  HAL_StatusTypeDef ret;

  //Set correct translation
  uint8_t buf[BLE_DATA_LENGTH];

  buf[0] = data->mode;
  buf[1] = data->ssr_id;
  buf[2] = data->air_time;
  buf[3] = (uint8_t) (data->env_temperature >> 8) & 0xFF; 
  buf[4] = (uint8_t) data->env_temperature & 0xFF; 
  buf[5] = data->env_humidity;
  buf[6] = (uint8_t) (data->env_lux >> 8) & 0xFF; 
  buf[7] = (uint8_t) data->env_lux & 0xFF; 
  buf[8] = (uint8_t) (data->dev_voltage >> 8) & 0xFF; 
  buf[9] = (uint8_t) data->dev_voltage & 0xFF; 
  buf[10] = data->dev_gyro_x;
  buf[11] = data->dev_gyro_y;
  buf[12] = data->dev_gyro_z;

  ret = _ble_send(hi2c1, BLE_ADDRESS, buf, sizeof(buf), 1000);

  if (ret == HAL_OK)
    return 1; // Good

  return 0; // Bad
}
void receive_ble_data(I2C_HandleTypeDef *hi2c1, uint8_t* buffer, uint8_t size)
{
  _ble_receive(hi2c1, BLE_ADDRESS, buffer, size, 1000);
}

uint8_t ble_device_ready(I2C_HandleTypeDef *hi2c1)
{
  return _ble_slave_available(hi2c1, BLE_ADDRESS, 1, 1000);
}

ble_beacon_result_t beacon(I2C_HandleTypeDef *hi2c1, ble_module_data_t* ble_data)
{
  ble_beacon_result_t beacon_result = {0};

  if(_wake_device) _wake_device();

  // wait until device is available
  while (ble_device_ready(hi2c1))
  {
    _delay_callback(10);
  }

  // Send out a value
  send_ble_data(hi2c1, ble_data);

  #ifdef DEBUG
  uint8_t Buffer[60] = {0};
  sprintf((char *)Buffer, "beacon - before %d \r\n", ble_data->air_time * 100+1000);
  serial_print(Buffer);
  #endif

  //Sleep for air_time
  //half_sleep(ble_data.air_time * 100);
  _delay_callback(ble_data->air_time * 100+1000);

  #ifdef DEBUG
  sprintf((char *)Buffer, "beacon - after \r\n");
  serial_print(Buffer);
  #endif

  uint8_t i = 0;
  //read scan data
  uint8_t received_data[2] = {0};
  do
  {
    //half_sleep(100);
    _delay_callback(1000);
    receive_ble_data(hi2c1, received_data, 2);

    #ifdef DEBUG
    sprintf((char *)Buffer, "beacon - scanning %d \r\n", i);
    serial_print(Buffer);
    #endif

    i++;
  }
  while((received_data[0] + received_data[1]) != 255 && i < 10);
  //Make sure the received value is correct based on the second value

  //Set the value in our own data struct
  #ifdef DEBUG
  sprintf((char *)Buffer, "beacon - End scanning %d \r\n", received_data[0]);
  serial_print(Buffer);
  #endif

  if(i<10)
    beacon_result.amount_of_ack = received_data[0];

  return beacon_result;
}

ble_scan_result_t scan(I2C_HandleTypeDef *hi2c1, ble_module_data_t* ble_data)
{
  ble_scan_result_t ble_scan_data = {0};
  if(_wake_device) _wake_device();

  // wait until device is available
  while (ble_device_ready(hi2c1))
  {
    _delay_callback(100);
  }

  // Send out the BLE data value
  send_ble_data(hi2c1, ble_data);

  _delay_callback(ble_data->air_time * 100 + 1000);

  //read scan data
  uint8_t i = 0;
  uint8_t received_data[12+1] = {0};
  do
  {
    _delay_callback(1000);
    receive_ble_data(hi2c1, received_data, 12+1);

    #ifdef DEBUG
    uint8_t Buffer[60] = {0};
    for (uint8_t i = 0; i < 13; i++)
    {
      sprintf((char *)Buffer, "d[%d] %d \t", i, received_data[i]);
      HAL_UART_Transmit(&huart2, (uint8_t *)Buffer, sizeof(Buffer), 1000);
    }
    sprintf((char *)Buffer, "\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)Buffer, sizeof(Buffer), 1000);
    #endif

    //Device is not responding
    // if (received_data[0] == 255 && received_data[12] == 255)
    //   continue;

    //No beacons found in the area
    if (received_data[0] == 0 && received_data[12] == 0)
      break;

    //Beacon found
    if (received_data[0] + received_data[12] == 255)
      break;
    //If the max tries have been reached
    i++;

    if(i >= 10)
      break;
  }
  while(1);
  //TODO add gyroscope

  //If all are 0 except for received_data[12], then no beacon found
  
  if (i < 10){
    ble_scan_data.ssr_id = received_data[0];
    ble_scan_data.env_temperature = received_data[1]>>8 | received_data[2];
    ble_scan_data.env_humidity = received_data[3];
    ble_scan_data.env_lux = received_data[4]>>8 | received_data[5];
    ble_scan_data.dev_voltage = received_data[6]>>8 | received_data[7];
    ble_scan_data.dev_gyro_x = received_data[8];
    ble_scan_data.dev_gyro_y = received_data[9];
    ble_scan_data.dev_gyro_z = received_data[10];
    ble_scan_data.rssi = received_data[11];
  }
  return ble_scan_data;
}