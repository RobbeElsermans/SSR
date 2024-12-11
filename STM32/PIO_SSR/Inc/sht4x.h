#ifndef SHT4X_H
#define SHT4X_H

#include "main.h"

// SHT4x I2C address
#define SHT4X_I2C_ADDR  0x44 << 1  // Default I2C address, shifted for HAL
#define SHT40_CMD_SOFT_RESET   0x94
// Commands
#define SHT4X_MEASURE_HIGH_PRECISION  0xFD
#define SHT4X_RESET                   0x94

// Function Prototypes
// HAL_StatusTypeDef SHT4x_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SHT4x_ReadTempAndHumidity(I2C_HandleTypeDef *hi2c, float *temperature, float *humidity);
void SHT40_Sleep(void);
void SHT40_ReadSensor(float* temp, float* hum);
#endif
