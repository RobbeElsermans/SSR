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

#define DATA_64 ((uint64_t)0x1234577712345777)
#define DATA_32 ((uint32_t)0x12345777)

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
struct ble_module_data_t ble_data;

uint8_t boolean_holder_1;
#define BOOL_SEND 0
#define BOOL_BEACON 1
#define BOOL_SCAN 2
#define BOOL_TRANSMIT 3
#define BOOL_SENS 4

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
  MCU_Init();

  // Debug UART
  MX_UART2_UART_Init();

  // If we came out a deep sleep state
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  // An initial startup from boot?
  else
  {
    
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

    /* Get the 1st page to erase */
    FirstPage = GetPage(FLASH_USER_START_ADDR);

    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

    /* Get the bank */
    BankNumber = GetBank(FLASH_USER_START_ADDR);

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = BankNumber;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
       you have to make sure that these data are rewritten before they are accessed during code
       execution. If this cannot be done safely, it is recommended to flush the caches by setting the
       DCRST and ICRST bits in the FLASH_CR register. */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
      /*
        Error occurred while page erase.
        User can add here some code to deal with this error.
        PAGEError will contain the faulty page and then to know the code error on this page,
        user can call function 'HAL_FLASH_GetError()'
      */
      /* Infinite loop */
      while (1)
      {
        /* Make LED3 blink (100ms on, 2s off) to indicate error in Erase operation */
        Error_Handler();
      }
    }

  /* Program the user Flash area word by word
  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, DATA_64) == HAL_OK)
    {
      Address = Address + 8;
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      while (1)
      {
        /* Make LED3 blink (100ms on, 2s off) to indicate error in Write operation */
        Error_Handler();
      }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;

  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4;
  }

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED3*/
    blink_led_times(1000, 2);
  }
  else
  {
    /* Error detected. LED3 will blink with 1s period */
    while (1)
    {
      Error_Handler();
    }
  }

  /* Configure RTC */
  if (RTC_Config())
  {
    Error_Handler();
  }

  // Blinky blinky
  blink_led(1000);

  deep_sleep(8000);

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

void beacon()
{
  BLE_Init();

  HAL_GPIO_WritePin(BLE_nSLEEP_GPIO_Port, BLE_nSLEEP_Pin, GPIO_PIN_SET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(BLE_nSLEEP_GPIO_Port, BLE_nSLEEP_Pin, GPIO_PIN_RESET);

  // wait until device is available
  while (ble_device_ready(&hi2c1))
  {
    // To test in power profiling
    // half_sleep(100);
  }

  // Add measurement to data struct
  ble_data.mode = 0;
  ble_data.air_time = 100; // 50*100 = 5000ms 5 second
  // ble_data.env_temperature++;
  // ble_data.env_humidity++;
  // ble_data.dev_voltage++;

  // Send out a value
  send_ble_data(&hi2c1, &ble_data);

  // Sleep for air_time
  // half_sleep(ble_data.air_time * 100);
  HAL_Delay(ble_data.air_time * 100);

  // read scan data
  uint8_t received_data[2] = {0};
  do
  {
    // half_sleep(100);
    HAL_Delay(100);
    receive_ble_data(&hi2c1, received_data, 2);
  } while ((received_data[0] + received_data[1]) != 255);
  // Make sure the received value is correct based on the second value

  // Set the value in our own data struct
  ble_beacon_data.amount_of_ack = received_data[0];
  // uint8_t Buffer[10] = {0};
  // sprintf(Buffer, "%d\r\n", received_data[0]);
  // HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);
}

void scan()
{
  BLE_Init();

  HAL_GPIO_WritePin(BLE_nSLEEP_GPIO_Port, BLE_nSLEEP_Pin, GPIO_PIN_SET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(BLE_nSLEEP_GPIO_Port, BLE_nSLEEP_Pin, GPIO_PIN_RESET);

  // wait until device is available
  while (ble_device_ready(&hi2c1))
  {
    // To test in power profiling
    // half_sleep(100);
  }

  // Add measurement to data struct
  ble_data.mode = 1;
  ble_data.air_time = 80; // 50*100 = 5000ms 5 second
  ble_data.env_temperature++;
  ble_data.env_humidity++;
  ble_data.dev_voltage++;

  // Send out the BLE data value
  send_ble_data(&hi2c1, &ble_data);

  // Sleep for air_time
  // half_sleep(ble_data.air_time * 100);
  HAL_Delay(ble_data.air_time * 100);

  // read scan data
  uint8_t received_data[9 + 1] = {0};
  do
  {
    // half_sleep(100);
    HAL_delay(100);
    receive_ble_data(&hi2c1, received_data, 9 + 1);
  } while (received_data[0] + received_data[9] == 255);

  // If all are 0 except for received_data[9], then no beacon found

  ble_scan_data.ssr_id = received_data[0];
  ble_scan_data.temperature = received_data[1] >> 8 | received_data[2];
  ble_scan_data.humidity = received_data[3];
  ble_scan_data.lux = received_data[4] >> 8 | received_data[5];
  ble_scan_data.voltage = received_data[6] >> 8 | received_data[7];
  ble_scan_data.rssi = received_data[8];

  uint8_t Buffer[10] = {0};
  sprintf(Buffer, "%d\r\n", ble_scan_data.rssi);
  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);
}
void sens()
{
  // TODO
}
void transmit()
{
  // TODO
}

void half_sleep(uint16_t time)
{
  uint16_t counter_value = 0x2806; // 5 seconds
  // Calculate the time
  if (time > 31981)
    counter_value = 0xFFFF; // Set maximum available time
  else
    counter_value = (uint16_t)((time / 1000.0) / 0.000488);

  // uint8_t Buffer[10] = {0};
  // sprintf(Buffer, "%02X", counter_value);
  // HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);

  // The stop 2 mode
  MX_RTC_Init();
  HAL_SuspendTick();
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, counter_value, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);
  // check_reg = seconds / 0.000488;

  /* Enter STOP 2 mode */
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
  SystemClock_Config();
  HAL_ResumeTick();
}

void deep_sleep(uint16_t time)
{
  /* Enter the Standby mode */
  if (lowPower_init(time))
  {
    Error_Handler();
  }
  else
  {
    HAL_PWR_EnterSTANDBYMode();
  }
  // Will not reach this code
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
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
}

void sensor_Init()
{
  LTR329_Init(&hi2c1);
  // SHT40_Init(&hi2c1);
}

void BLE_Init()
{
  MX_I2C1_Init();
}
void LoRa_Init()
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