/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIO5E_H
#define __WIO5E_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "usart.h"

typedef void (*delay_callback_t)(uint32_t time);
void loraDelayCallback(delay_callback_t dc_fp);
extern delay_callback_t _delay_callback;

// Methods
void testLoRa();
void setupLoRa();
void send_data_over_lora(ssr_data_t data);
void write_read_command(char* command, char* check);

#ifdef __cplusplus
}
#endif

#endif
