#include "sht4x.h"
#include <stdio.h>
#include "i2c.h"
#include "usart.h"
#define SHT40_I2C_ADDR (0x44 << 1)
#define SHT40_SLEEP_CMD 0xB9
// Soft reset command
// Function to initialize the sensor (soft reset)
void SHT40_Sleep(void)
{
  HAL_StatusTypeDef ret;
  uint8_t sleep_cmd = SHT40_SLEEP_CMD;

  // Transmit the sleep command
  ret = HAL_I2C_Master_Transmit(&hi2c1, SHT40_I2C_ADDR, &sleep_cmd, 1, 1000);
  if (ret != HAL_OK)
  {
    // Handle transmission error
    char error_msg[50];
    sprintf(error_msg, "SHT40 Sleep Command Failed: %d\r\n", ret);
    HAL_UART_Transmit(&huart2, (uint8_t *)error_msg, strlen(error_msg), HAL_MAX_DELAY);
  }
  else
  {
    HAL_UART_Transmit(&huart2, (uint8_t *)"SHT40 is now in Sleep Mode.\r\n", 29, HAL_MAX_DELAY);
  }
}

void SHT40_ReadSensor(float* temp, float* hum)
{

  HAL_StatusTypeDef ret;
  uint8_t data_tx[1] = {0xFD};
  uint8_t data_rx[6];
  ret = HAL_I2C_Master_Transmit(&hi2c1, 0x44 << 1, data_tx, 1, 10);
  if (ret != HAL_OK)
  {
    char Buffer[25] = {0};
    sprintf(Buffer, "Error\r\n");
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10);
    HAL_Delay(1000);
  }
  else
  {
    // read bytes
    HAL_Delay(1000);
    ret = HAL_I2C_Master_Receive(&hi2c1, 0x44 << 1, data_rx, 6, 10);
    if (ret != HAL_OK)
    {
      char Buffer[25] = {0};
      sprintf(Buffer, "Error\r\n");
      HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10);
      HAL_Delay(1000);
    }
    else
    {
      for (int i = 0; i < 6; i++)
      {
        char Buffer[25] = {0};
        sprintf("data_rx[%i] = %u \n", i, data_rx[i]);
        HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10);
      }
      float t_ticks = data_rx[0] * 256 + data_rx[1];
      float rh_ticks = data_rx[3] * 256 + data_rx[4];

      *temp = -45 + 175 * t_ticks / 65535;
      *hum = -6 + (125.0 * rh_ticks) / 65535.0;

      char Buffer[25] = {0};

      sprintf(Buffer, "temp: %f, hum :  %f\r\n", *temp, *hum);
      HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10);
      HAL_Delay(100);
    }
  }
}
