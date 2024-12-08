/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

extern struct ble_module_data_t ble_data;

/* Exported constants --------------------------------------------------------*/
#define MPU6050_DEV_ADDR          0x68 // I2C address of the MPU6050

#define MPU6050_SMPLRT_DIV        0x19 // sample rate divisor register
#define MPU6050_CONFIG            0x1A // General configuration register
#define MPU6050_GYRO_CONFIG       0x1B // Gyro specfic configuration register
#define MPU6050_ACCEL_CONFIG      0x1C // Accelerometer specific configration register
#define MPU6050_MOT_THR           0x1F // Motion detection threshold bits [7:0]
#define MPU6050_FIFO_EN           0x23 // Which sensor measurements are loaded into the FIFO buffer
#define MPU6050_INT_PIN_CONFIG    0x37 // Interrupt pin configuration register
#define MPU6050_INT_ENABLE        0x38 // Interrupt enable configuration register
#define MPU6050_GYRO_OUT          0x43 // Base address for gyroscope sensor data reads
#define MPU6050_USER_CTRL         0x6A // FIFO and I2C Master control register
#define MPU6050_PWR_MGMT_1        0x6B // Primary power/sleep control register
#define MPU6050_PWR_MGMT_2        0x6C // Secondary power/sleep control register
#define MPU6050_WHO_AM_I          0x75 // Divice ID register

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void MCU_Init();
void sensor_Init();
void BLE_Init();
void LoRa_Init();

// I2C
HAL_StatusTypeDef ret;
uint8_t data_tx[1];
uint8_t data_rx[6];

// MPU6050
void setMPU6050();

//tasks
void beacon();
void scan();
void sens();
void transmit();

//sleep modes
void half_sleep(uint16_t time);
void deep_sleep(uint16_t time);

//Helpers
void blink_led_times(uint16_t time, uint8_t times);
void blink_led(uint16_t time);
void setBool(uint8_t *bool_carrier, uint8_t bool_place);
void clearBool(uint8_t *bool_carrier, uint8_t bool_place);
uint8_t checkBool(uint8_t *bool_carrier, uint8_t bool_place);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCP_TX_Pin        GPIO_PIN_2
#define VCP_TX_GPIO_Port  GPIOA
#define SWDIO_Pin         GPIO_PIN_13
#define SWDIO_GPIO_Port   GPIOA
#define SWCLK_Pin         GPIO_PIN_14
#define SWCLK_GPIO_Port   GPIOA
#define VCP_RX_Pin        GPIO_PIN_15
#define VCP_RX_GPIO_Port  GPIOA

#define USER_LED_Pin          GPIO_PIN_3
#define USER_LED_GPIO_Port    GPIOB
#define BLE_nSLEEP_Pin        GPIO_PIN_6
#define BLE_nSLEEP_GPIO_Port  GPIOB

#define SSR_ID 0x10

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
