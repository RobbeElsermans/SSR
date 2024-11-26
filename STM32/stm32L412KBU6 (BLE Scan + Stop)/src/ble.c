#include "ble.h"

uint8_t send_ble_data(I2C_HandleTypeDef *hi2c1, struct ble_module_data_t *data)
{
    HAL_StatusTypeDef ret;

    //Set correct translation
    uint8_t buf[13];

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
    buf[10] = data->ssr_id;
    buf[11] = data->ssr_id;
    buf[12] = data->ssr_id;

    ret = HAL_I2C_Master_Transmit(hi2c1, BLE_ADDRESS, buf, sizeof(buf), HAL_MAX_DELAY);

    if (ret == HAL_OK)
        return 1; // Good

    return 0; // Bad
}

uint8_t receive_ble_data(I2C_HandleTypeDef *hi2c1)
{
    uint8_t buffer[1];
    HAL_I2C_Master_Receive(hi2c1, BLE_ADDRESS, buffer, sizeof(buffer), 1);
    if(buffer[0] == 0)
    {
        return 0;
    }
    else{
        return buffer[0];
    }
}

uint8_t ble_device_ready(I2C_HandleTypeDef *hi2c1){
    uint8_t ret = HAL_I2C_IsDeviceReady(hi2c1, BLE_ADDRESS, 1, 1);
    return ret;
}