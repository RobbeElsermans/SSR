#ifndef __SHT40_H__
#define __SHT40_H__

#include "stm32l4xx_hal.h"
#include "stdint.h"
#include "stdlib.h"

// SHT4x I2C address
#define SHT40_I2C_ADDR  0x44 << 1  // Default I2C address, shifted for HAL

// Commands

#define SHT40_HIGH_PRECISION    0
#define SHT40_MEDIUM_PRECISION  1
#define SHT40_LOW_PRECISION     2

#define SHT40_REG_MEASURE_HIGH_PRECISION    0xFD
#define SHT40_REG_MEASURE_MEDIUM_PRECISION  0xF6
#define SHT40_REG_MEASURE_LOW_PRECISION     0xE0
#define SHT40_REG_SOFT_RESET                0x94

//Heater is not used

typedef void (*delay_callback_t)(uint32_t time);
void sht40DelayCallback(delay_callback_t dc_fp);
extern delay_callback_t _delay_callback;

HAL_StatusTypeDef sht40Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef sht40ReadTempAndHumidity(I2C_HandleTypeDef *hi2c, float *temperature, float *humidity, uint8_t precision);
HAL_StatusTypeDef sht40Sleep(I2C_HandleTypeDef *hi2c);

#endif
