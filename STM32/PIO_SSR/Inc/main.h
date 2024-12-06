/* USER CODE BEGIN Header */
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct ssr_data
{
  uint16_t seq_number;      // Range from 0 to 511 (8 bits total usage)
  int16_t env_temperature;  // Range from -327.68 to 327.67 °C (val/100=°C)
  uint8_t env_humidity;     // Range from -0-100%
  uint16_t env_lux;         // Range from 0 to 1000
  uint16_t dev_voltage;     // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  int8_t gyro_x;            // Range from -60 to 60 (val*3=°)
  int8_t gyro_y;            // Range from -60 to 60 (val*3=°)
  int8_t gyro_z;            // Range from -60 to 60 (val*3=°)
};
typedef struct ssr_data ssr_data_t;
extern ssr_data_t ssr_data;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void wakeBleModule();
void wakeltrModule();
void sleepltrModule();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LB_F_Pin GPIO_PIN_1
#define LB_F_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_3
#define VBAT_GPIO_Port GPIOA
#define LB_R_Pin GPIO_PIN_4
#define LB_R_GPIO_Port GPIOA
#define LB_L_Pin GPIO_PIN_5
#define LB_L_GPIO_Port GPIOA
#define LB_RA6_Pin GPIO_PIN_6
#define LB_RA6_GPIO_Port GPIOA
#define LB_S_Pin GPIO_PIN_7
#define LB_S_GPIO_Port GPIOA
#define LoRa_EN_Pin GPIO_PIN_8
#define LoRa_EN_GPIO_Port GPIOA
#define LoRa_TX_Pin GPIO_PIN_9
#define LoRa_TX_GPIO_Port GPIOA
#define LoRa_RX_Pin GPIO_PIN_10
#define LoRa_RX_GPIO_Port GPIOA
#define SHT40_EN_Pin GPIO_PIN_11
#define SHT40_EN_GPIO_Port GPIOA
#define LTR329_EN_Pin GPIO_PIN_12
#define LTR329_EN_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_3
#define USER_LED_GPIO_Port GPIOB
#define BLE_EN_Pin GPIO_PIN_5
#define BLE_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
