#ifndef _BLE_module_
#define _BLE_module_

#include "main.h"

#define BLE_ADDRESS 0x12 << 1 

struct ble_module_data_t
{
    uint8_t mode;             // The mode of the BLE-module, 0 -> beacon, 1-> scan
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

uint8_t send_ble_data(I2C_HandleTypeDef *hi2c1, struct ble_module_data_t *data);
uint8_t ble_device_ready(I2C_HandleTypeDef *hi2c1);

#endif