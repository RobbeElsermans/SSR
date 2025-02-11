/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
HAL_StatusTypeDef ret;

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;
HAL_StatusTypeDef ret;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void I2C_Scan() {
  #ifdef DEBUG
  char Buffer[25] = {0};
  char Space[] = " - ";
  char StartMSG[] = "Starting I2C Scanning: \r\n";
  char EndMSG[] = "Done! \r\n\r\n";
  #endif

  uint8_t i = 0, ret;

  #ifdef DEBUG
  serial_print(StartMSG);
  #endif
  for (i = 0; i < 128; i++) {
    ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 3, 5);
    if (ret != HAL_OK) { /* No ACK Received At That Address */
      Error_Handler();
      #ifdef DEBUG
      serial_print(Space);
      #endif
    } else if (ret == HAL_OK) {
      #ifdef DEBUG
      sprintf(Buffer, "0x%X", i);
      serial_print(Buffer);
      #endif
    }
  }
  #ifdef DEBUG
  serial_print(EndMSG);
  #endif
}

void i2c_write(uint8_t address, uint8_t* data_tx, uint8_t tx_size) {
  ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
  if (ret != HAL_OK) {
    Error_Handler();
    #ifdef DEBUG
    char Buffer[8] = {"Error\r\n"};
    serial_print(Buffer);
    HAL_Delay(1000);
    #endif
  }
}

void i2c_write_read(uint8_t address, uint8_t* data_tx, uint8_t tx_size, uint8_t* data_rx, uint8_t rx_size) {
  ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
  if (ret != HAL_OK) {
    Error_Handler();
    #ifdef DEBUG
    char Buffer[8] = {"Error\r\n"};
    serial_print(Buffer);
    HAL_Delay(1000);
    #endif
  } else {
    // read bytes
    //HAL_Delay(1000);
    ret = HAL_I2C_Master_Receive(&hi2c1, address << 1, data_rx, rx_size, 1000);
    if (ret != HAL_OK) {
      Error_Handler();
      #ifdef DEBUG
      char Buffer[8] = {"Error\r\n"};
      serial_print(Buffer);
      HAL_Delay(1000);
      #endif
    } else {
      #ifdef DEBUG
      for (int i = 0; i < rx_size; i++) {
        char Buffer[16] = {0};
        sprintf(Buffer, "data_rx[%i] = %02X\n", i, data_rx[i]);
        serial_print(&Buffer);
      }
      #endif
    }
  }
}

/* USER CODE END 1 */
