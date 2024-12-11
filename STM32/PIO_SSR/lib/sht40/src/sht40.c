#include "sht40.h"
#include "usart.h"

delay_callback_t _delay_callback;
void sht40DelayCallback(delay_callback_t dc_fp)
{
  _delay_callback = dc_fp;
}

HAL_StatusTypeDef sht40Init(I2C_HandleTypeDef *hi2c)
{
  // Transmit the sleep command
  return sht40Sleep(hi2c);
}

HAL_StatusTypeDef sht40ReadTempAndHumidity(I2C_HandleTypeDef *hi2c, float *temperature, float *humidity, uint8_t precision)
{
  HAL_StatusTypeDef ret;
  uint8_t wait_duration = 0;
  uint8_t reg_address = 0;
  if (precision == 0)
  {
    reg_address = SHT40_REG_MEASURE_HIGH_PRECISION;
    wait_duration = 10;
  }
  else if (precision == 1)
  {
    reg_address = SHT40_REG_MEASURE_MEDIUM_PRECISION;
    wait_duration = 6;
  }
  else
  {
    reg_address = SHT40_REG_MEASURE_LOW_PRECISION;
    wait_duration = 2;
  }

  ret = HAL_I2C_Master_Transmit(hi2c, SHT40_I2C_ADDR, &reg_address, 1, 1);
  if (ret != HAL_OK)
  {
    return ret;
  }

  _delay_callback(wait_duration);

  uint8_t buffer[6];
  ret = HAL_I2C_Master_Receive(hi2c, SHT40_I2C_ADDR, (uint8_t *)buffer, 6, 10);

  if (ret != HAL_OK)
  {
    return ret;
  }

  uint8_t Buffer[60] = {0};
  sprintf(Buffer, "taskSens - hum: %d\r\n",  -45 + 175 * (buffer[0] * 256 + buffer[1])/ 65535);
  serial_print(Buffer);

  float t_ticks = buffer[0] * 256 + buffer[1];
  float rh_ticks = buffer[3] * 256 + buffer[4];

  uint8_t bufuffer[60] = {0};
  sprintf(bufuffer, "taskSens - hum: %d\r\n", -45 + 175 * t_ticks / 65535);
  serial_print(bufuffer);

  *temperature = -45 + 175 * t_ticks / 65535;
  *humidity = -6 + (125.0 * rh_ticks) / 65535.0;

  return HAL_OK;
}

HAL_StatusTypeDef sht40Sleep(I2C_HandleTypeDef *hi2c)
{
  return HAL_I2C_Master_Transmit(hi2c, SHT40_I2C_ADDR, (uint8_t *)SHT40_REG_SOFT_RESET, 1, 1);
}