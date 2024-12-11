#include "ltr_329.h"
#include "usart.h"

void LTR329_Init(I2C_HandleTypeDef* hi2c1)
{

  uint8_t data[2];

  // Enable ALS sensor, set to active mode
  data[0] = LTR329_ALS_CONTR;
  data[1] = 0x01; // Set to active mode with default gain
  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
  HAL_Delay(10); // Wait for wakeup

  // Set ALS measurement rate and integration time
  data[0] = LTR329_ALS_MEAS_RATE;
  data[1] = 0x13; // 200ms integration, 500ms measurement rate
  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
  HAL_Delay(100); // Initial startup time
}

uint16_t LTR329_ReadALS(I2C_HandleTypeDef* hi2c1, uint8_t channel)
{
  uint8_t regAddr;
  uint8_t rawData[2];
  uint16_t data;

  // Select register address based on the channel
  if (channel == 0)
  {
    regAddr = LTR329_ALS_DATA_CH0_0;
  }
  else if (channel == 1)
  {
    regAddr = LTR329_ALS_DATA_CH1_0;
  }
  else
  {
    return 0; // Invalid channel
  }

  // Read 2 bytes (low and high) from the sensor

  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  HAL_Delay(10);
  HAL_I2C_Master_Receive(hi2c1, LTR329_I2C_ADDR, &rawData[0], 1, HAL_MAX_DELAY);
  HAL_Delay(10);
  regAddr++;
  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  HAL_Delay(10);
  HAL_I2C_Master_Receive(hi2c1, LTR329_I2C_ADDR, &rawData[1], 1, HAL_MAX_DELAY);

  data = (rawData[1] << 8) | rawData[0]; // Combine low and high bytes

  // uint8_t Buffer[60] = {0};
  // sprintf(Buffer, "taskSens - lux: %d\r\n", data);
  // HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);

  return data;
}

uint16_t GetLuxAll(I2C_HandleTypeDef* hi2c1)
{
    //Accordingly to datasheet, read them both.
    uint16_t ch1 = LTR329_ReadALS(hi2c1, 1); // Read channel 1 data

    return ch1;
}

uint16_t GetLuxIR(I2C_HandleTypeDef* hi2c1)
{
    //Accordingly to datasheet, read them both.
    uint16_t ch0 = LTR329_ReadALS(hi2c1, 0); // Read channel 0 data

    return ch0;
}

void LTR329_Sleep(I2C_HandleTypeDef* hi2c1) {
    uint8_t data[2];
    
    // Write to ALS_CONTR register to set the sensor in standby mode (0x00).
    data[0] = LTR329_ALS_CONTR;
    data[1] = 0x00; // Standby mode
    HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
}

void LTR329_WakeUp(I2C_HandleTypeDef* hi2c1) {
    uint8_t data[2];

    // Write to ALS_CONTR register to set the sensor in active mode.
    data[0] = LTR329_ALS_CONTR;
    data[1] = 0x01; // Active mode with default gain
    HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);

    // Wait for the sensor to wake up.
    HAL_Delay(10); // According to the sensor datasheet
}