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

extern uint8_t data_send;

// Methods
void testLoRa();
void initLoRa();
void sleepLoRa(uint8_t sleep);
void send_data_over_lora(ssr_data_t data);
void write_read_command_1000(char* command, char* check);
void write_read_command(char* command, char* check, uint16_t delay);
void write_command(char* command);

#ifdef __cplusplus
}
#endif

#endif
