/**
 ******************************************************************************
 * @file    PWR/PWR_STANDBY_RTC/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use STM32L4xx PWR HAL API to enter
 *          and exit the Standby mode using RTC.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "lp.h"
#include "ble.h"
#include "rtc.h"

/* Flash as EEPROM */
#define FLASH_USER_START_ADDR ADDR_FLASH_PAGE_60                     /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1 /* End @ of user Flash area */

#define START_COUNTER_REG (FLASH_USER_START_ADDR + 0x00000000)
// #define END_COUNTER_REG 0x080001FF
#define END_COUNTER_REG (FLASH_USER_START_ADDR + 0x00000001) // 1 -> 64bit -> high number
#define START_EEPROM_REG (FLASH_USER_START_ADDR + 0x00000002)
#define STEP_EEPROM_REG 0x1 // 64 byte space between.

#define DATA_64 ((uint64_t)0x0000000000000000)
#define DATA_32 ((uint32_t)0x00000000)

uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0, MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);
/* Flash as EEPROM */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // Debug UART
  MX_UART2_UART_Init();

  // Check the value of the first 9 bit value. Therefore, read out a full 32-bytes object and check this
  uint16_t counter = 0;
  Address = START_COUNTER_REG;
  data32 = *(__IO uint32_t *)Address;
  // In the first 2 bytes, the counter is present
  counter = (uint16_t)(data32 & 0xFFFF);

  counter++;

// See below for further development
// https://hackaday.io/project/177197-the-slowest-video-player-with-7-colors/log/196302-stm32-standby-mode-flash-vs-backup-sram
// https://github.com/ts-manuel/VideoFrame/blob/master/stm32/Core/Src/hardware/flash.c
  // Save to flash

  HAL_FLASH_Unlock();
  Address = START_COUNTER_REG;

  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, (uint64_t)counter) == HAL_OK)
  {
    Address = Address + 8;
  }
  else
  {
    while (1)
      Error_Handler();
  }

  HAL_FLASH_Lock();

  /* Unlock the Flash to enable the flash control register access *************/
  // HAL_FLASH_Unlock();
  // FirstPage = GetPage(FLASH_USER_START_ADDR);
  // NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
  // BankNumber = GetBank(FLASH_USER_START_ADDR);

  // /* Fill EraseInit structure*/
  // EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  // EraseInitStruct.Banks = BankNumber;
  // EraseInitStruct.Page = FirstPage;
  // EraseInitStruct.NbPages = NbOfPages;

  // if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) while (1) Error_Handler();

  // /* Program the user Flash area word by word
  // (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  // Address = FLASH_USER_START_ADDR;
  // while (Address < FLASH_USER_END_ADDR)
  // {
  //   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, DATA_64) == HAL_OK)
  //   {
  //     Address = Address + 8;
  //   }
  //   else
  //   {
  //     while (1)
  //       Error_Handler();
  //   }
  // }

  // HAL_FLASH_Lock();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  // Address = FLASH_USER_START_ADDR;
  // MemoryProgramStatus = 0x0;

  // while (Address < FLASH_USER_END_ADDR)
  // {
  //   data32 = *(__IO uint32_t *)Address;

  //   if (data32 != DATA_32)
  //   {
  //     MemoryProgramStatus++;
  //   }
  //   Address = Address + 4; //Shift 4 bytes
  // }

  // /*Check if there is an issue to program data*/
  // if (MemoryProgramStatus == 0) blink_led_times(1000, 2);
  // else while (1) Error_Handler();

  /* Configure RTC */
  if (RTC_Config())
    Error_Handler();

  // Blinky blinky
  blink_led(1000);

  // deep_sleep(8000);

  while (1)
  {
    blink_led(50);
  }
}

/**
 * @brief  Gets the page of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The page of a given address
 */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

/**
 * @brief  Gets the bank of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The bank of a given address
 */
static uint32_t GetBank(uint32_t Addr)
{
  return FLASH_BANK_1;
}

void blink_led(uint16_t time)
{
  blink_led_times(time, (uint8_t)1);
}

void blink_led_times(uint16_t time, uint8_t times)
{
  for (uint8_t i = 0; i < times; i++)
  {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    HAL_Delay(time);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    HAL_Delay(time);
  }
}

// Mandatory to operate MCU in the first place
void MCU_Init()
{
}

void setBool(uint8_t *bool_carrier, uint8_t bool_place)
{
  *bool_carrier |= 0b1 << bool_place;
}

void clearBool(uint8_t *bool_carrier, uint8_t bool_place)
{
  *bool_carrier &= ~(0b1 << bool_place);
}

uint8_t checkBool(uint8_t *bool_carrier, uint8_t bool_place)
{
  return *bool_carrier & (0b1 << bool_place);
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follows :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 4000000
 *            PLL_M                          = 1
 *            PLL_N                          = 40
 *            PLL_R                          = 2
 *            PLL_Q                          = 4
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while (1)
      ;
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while (1)
      ;
  }
}

/**
 * @brief SYSTICK callback
 * @param None
 * @retval None
 */
void HAL_SYSTICK_Callback(void)
{
  HAL_IncTick();
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
  MX_GPIO_Init();
  while (1)
  {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
  }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif