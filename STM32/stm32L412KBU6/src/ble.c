#include "ble.h"

uint8_t send_ble_data(I2C_HandleTypeDef *hi2c1, struct ble_module_data_t *data)
{
    HAL_StatusTypeDef ret;

    //Set correct translation
    uint8_t buf[13];

    buf[0] = data->ssr_id;
    buf[1] = data->beacon_time;
    buf[2] = data->scan_time;
    buf[3] = (uint8_t) (data->env_temperature >> 8) & 0xFF; 
    buf[4] = (uint8_t) data->env_temperature & 0xFF; 
    buf[5] = data->env_humidity;
    buf[6] = (uint8_t) (data->env_lux >> 8) & 0xFF; 
    buf[7] = (uint8_t) data->env_lux & 0xFF; 
    buf[8] = (uint8_t) (data->dev_voltage >> 8) & 0xFF; 
    buf[9] = (uint8_t) data->dev_voltage & 0xFF; 
    buf[10] = data->ssr_id;
    buf[11] = data->ssr_id;
    buf[12] = data->ssr_id;

    ret = HAL_I2C_Master_Transmit(hi2c1, BLE_ADDRESS, buf, sizeof(buf), HAL_MAX_DELAY);

    if (ret == HAL_OK)
        return 1; // Good

    return 0; // Bad
}