#ifndef LP_H
#define LP_H

#include "main.h"
//#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0x00F9  /* 32 kHz RC/128 - 1 */

void GPIO_Disable(void);
uint8_t RTC_Config(void);
uint8_t lowPower_init(uint16_t time);

#ifdef __cplusplus
}
#endif

#endif