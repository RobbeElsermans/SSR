#ifndef _LDR_329_
#define _LDR_329_

#include "stm32l4xx_hal.h"
#include "stdint.h"

//extern I2C_HandleTypeDef* hi2c1;
#define LTR329_I2C_ADDR (0x29 << 1) // I2C address shifted for HAL

#define LTR329_ALS_CONTR 0x80      // ALS control register
#define LTR329_ALS_MEAS_RATE 0x85  // ALS measurement rate register
#define LTR329_ALS_DATA_CH1_0 0x88 // ALS Channel 1 low byte
#define LTR329_ALS_DATA_CH1_1 0x89 // ALS Channel 1 high byte
#define LTR329_ALS_DATA_CH0_0 0x8A // ALS Channel 0 low byte
#define LTR329_ALS_DATA_CH0_1 0x8B // ALS Channel 0 high byte

typedef void (*delay_callback_t)(uint32_t time);
void ltrDelayCallback(delay_callback_t dc_fp);
extern delay_callback_t _delay_callback;

// typedef void (*wake_device_t)();
// void ltrWakeCallback(wake_device_t wake_device_fp);
// extern wake_device_t _wake_device;

// typedef void (*sleep_device_t)();
// void ltrSleepCallback(sleep_device_t sleep_device_fp);
// extern sleep_device_t _sleep_device;

extern uint8_t device_active;

void ltr329Init(I2C_HandleTypeDef* i2c_obj);
HAL_StatusTypeDef ltr329ReadALS(I2C_HandleTypeDef* i2c_obj, uint8_t channel, uint16_t* lux);
HAL_StatusTypeDef ltr329GetLuxAll(I2C_HandleTypeDef* i2c_obj, uint16_t* lux);
HAL_StatusTypeDef ltr329GetLuxIR(I2C_HandleTypeDef* i2c_obj, uint16_t* lux);
HAL_StatusTypeDef ltr329Sleep(I2C_HandleTypeDef* hi2c1);
HAL_StatusTypeDef ltr329WakeUp(I2C_HandleTypeDef* hi2c1);
// void ltrSleep();

#endif