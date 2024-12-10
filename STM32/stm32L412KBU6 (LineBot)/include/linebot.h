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

// Methods
void driveLineBot(DRIVE_STATE action);

#ifdef __cplusplus
}
#endif

#endif
