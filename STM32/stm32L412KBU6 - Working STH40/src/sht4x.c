#include "sht4x.h"
#include <stdio.h>
#define SHT40_I2C_ADDR         (0x44 << 1) 
         // Soft reset command
// Function to initialize the sensor (soft reset)
 

    void LedBlinks()
    {
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
    }







