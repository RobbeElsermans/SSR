/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LINEBOT_H
#define __LINEBOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

// LineBot pins
#define LB_B_Pin      GPIO_PIN_4
#define LB_L_Pin      GPIO_PIN_5
#define LB_F_Pin      GPIO_PIN_1
#define LB_S_Pin      GPIO_PIN_7
#define LB_R_Pin      GPIO_PIN_6
#define LB_GPIO_Port  GPIOA

typedef enum {
    STOP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
} DRIVE_STATE;

// Methods
void driveLineBot(DRIVE_STATE action);

#ifdef __cplusplus
}
#endif

#endif
