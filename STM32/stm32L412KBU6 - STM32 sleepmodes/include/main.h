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

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void MCU_Init();
void sensor_Init();
void BLE_Init();
void LoRa_Init();

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
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA

#define USER_LED_Pin GPIO_PIN_3
#define USER_LED_GPIO_Port GPIOB
#define BLE_nSLEEP_Pin GPIO_PIN_6
#define BLE_nSLEEP_GPIO_Port GPIOB

#define SSR_ID 0x10

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
