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
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

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
#define LD3_Pin           GPIO_PIN_3
#define LD3_GPIO_Port     GPIOB

// LineBot pins
#define LB_B_Pin      GPIO_PIN_4
#define LB_L_Pin      GPIO_PIN_5
#define LB_F_Pin      GPIO_PIN_1
#define LB_S_Pin      GPIO_PIN_7
#define LB_R_Pin      GPIO_PIN_6
#define LB_GPIO_Port  GPIOA

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
