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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

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
  // Set ID of the SSR
  ble_data.ssr_id = SSR_ID;

  MCU_Init();

  // Debug UART
  MX_UART2_UART_Init();

  /* Configure RTC */
  if (RTC_Config())
  {
    Error_Handler();
  }

  BLE_Init();

  uint8_t counter = 0;
  uint8_t buffer[4];

  while (1)
  {
    //Size is available in the onReceive method in Wire.

    HAL_I2C_Master_Transmit(&hi2c1, BLE_ADDRESS, &counter, 2, 1);
    HAL_Delay(1000);

    //Size is not communicated. This is pure for the local buffer.
    //So know whath to send back to the STM32 from the slave based on previously sended values?
    HAL_I2C_Master_Receive(&hi2c1, BLE_ADDRESS, buffer, sizeof(buffer), 1);

    uint8_t Buffer[20] = {0};
    sprintf(Buffer, "tx:%d, rx:%d,%d,%d,%d \r\n",counter, buffer[0],buffer[1],buffer[2],buffer[3]);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);

    counter = buffer[0];
    counter++;
  }
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
  ble_data.air_time = 50; // 50*100 = 5000ms 5 second
  // ble_data.env_temperature++;
  // ble_data.env_humidity++;
  // ble_data.dev_voltage++;

  // Send out a value
  send_ble_data(&hi2c1, &ble_data);

  // Sleep for air_time
  // half_sleep(ble_data.air_time * 100);
  HAL_Delay(ble_data.air_time * 100);

  // read scan data
  uint8_t received_data = 0;
  do
  {
    // half_sleep(100);
    HAL_Delay(100);
    received_data = receive_ble_data(&hi2c1);
  } while (received_data == 0);

  uint8_t Buffer[10] = {0};
  sprintf(Buffer, "%d\r\n", received_data);
  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);
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
  half_sleep(ble_data.air_time * 100);

  // read scan data
  uint8_t received_data = 0;
  do
  {
    half_sleep(100);
    received_data = receive_ble_data(&hi2c1);
  } while (received_data == 0);

  uint8_t Buffer[10] = {0};
  sprintf(Buffer, "%d\r\n", received_data);
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
    HAL_Delay(500);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
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