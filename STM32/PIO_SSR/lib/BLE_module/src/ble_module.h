#ifndef __BLE_MODULE__
#define __BLE_MODULE__

#include "stm32l4xx_hal.h"
#include "stdint.h"

// #define DEBUG

#define BLE_ADDRESS (0x12 << 1) //The devices' address shifted so that the R/W bit can be set.
#define BLE_DATA_LENGTH 13 //Amount of bytes present in the struct

struct ble_module_data
{
  uint8_t mode; // The mode of the BLE-module, 0 -> beacon, 1-> scan
  uint8_t ssr_id; // The ID of the rover itself
  uint8_t air_time; // How long the beacon may last (val*100=ms)
  int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
  uint8_t env_humidity; // Range from -0-100%
  uint16_t env_lux; // Range from 0 to 1000
  uint16_t dev_voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  int8_t dev_gyro_x; // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_y; // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_z; // Range from -60 to 60 (val*3=°)
};
typedef struct ble_module_data ble_module_data_t;

struct ble_beacon_result
{
  // How many devices have acked the Beacon. this determines the amount of nodes present
  uint8_t amount_of_ack;
};
typedef struct ble_beacon_result ble_beacon_result_t;

struct ble_scan_result
{
  uint8_t ssr_id;         // The ID of the source
  int16_t env_temperature;// temperature
  uint8_t env_humidity;   // humidity
  uint16_t env_lux;       // lux (light)
  uint16_t dev_voltage;   // voltage
  int8_t rssi;            //rssi
  int8_t dev_gyro_x;      // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_y;      // Range from -60 to 60 (val*3=°)
  int8_t dev_gyro_z;      // Range from -60 to 60 (val*3=°)
};
typedef struct ble_scan_result ble_scan_result_t ;

typedef void (*delay_callback_t)(uint32_t time);  //Type definition
void bleDelayCallback(delay_callback_t dc_fp);       //callback function definition
extern delay_callback_t _delay_callback;          //callback pointer for in code usage

typedef HAL_StatusTypeDef (*ble_transmit_t)(I2C_HandleTypeDef* hi2c, uint16_t dev_addr, uint8_t* data_to_send, uint16_t data_size_bytes, uint32_t timeout);
void bleSendCallback(ble_transmit_t ble_transmit_fp);
extern ble_transmit_t _ble_send;

typedef HAL_StatusTypeDef (*ble_receive_t)(I2C_HandleTypeDef* hi2c, uint16_t dev_addr, uint8_t* data_buffer, uint16_t data_size_bytes, uint32_t timeout);
void bleReceiveCallback(ble_receive_t ble_receive_fp);
extern ble_receive_t _ble_receive;

typedef HAL_StatusTypeDef (*ble_slave_available_t)(I2C_HandleTypeDef* hi2c, uint16_t dev_addr, uint32_t retries, uint32_t timeout);
void bleAvailableCallback(ble_slave_available_t ble_available_fp);
extern ble_slave_available_t _ble_slave_available;

typedef void (*wake_device_t)();
void bleWakeCallback(wake_device_t wake_device_fp);
extern wake_device_t _wake_device;

uint8_t send_ble_data(I2C_HandleTypeDef *hi2c1, ble_module_data_t *data);
void receive_ble_data(I2C_HandleTypeDef *hi2c1, uint8_t* buffer, uint8_t size);
uint8_t ble_device_ready(I2C_HandleTypeDef *hi2c1);

ble_beacon_result_t beacon(I2C_HandleTypeDef *hi2c1, ble_module_data_t* ble_data);
ble_scan_result_t scan(I2C_HandleTypeDef *hi2c1, ble_module_data_t* ble_data);

#endif