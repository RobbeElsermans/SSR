#ifndef __LP_H__
#define __LP_H__

#include "stm32l412xx.h"
#include "stm32l4xx_hal_rtc_ex.h"
#include "stdint.h"

uint16_t counter_value(uint16_t time_millis);
void half_sleep(uint16_t time);
void deep_sleep(uint16_t time);

#endif