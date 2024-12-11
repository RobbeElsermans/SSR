/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LINEBOT_H
#define __LINEBOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "gpio.h"

typedef enum {
    STOP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
} DRIVE_STATE;

typedef void (*delay_callback_t)(uint32_t time);
void lineBotDelayCallback(delay_callback_t dc_fp);
extern delay_callback_t _delay_callback;

// Methods
void setupLineBot();
void driveLineBot(DRIVE_STATE action);

#ifdef __cplusplus
}
#endif

#endif
