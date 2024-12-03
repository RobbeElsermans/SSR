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

  /* Check if the system was resumed from StandBy mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    //blink_led(100);
  }

  while(1)
  {
    // Blinky blinky
    blink_led(1000);
    HAL_delay(2000);
    //half_sleep(5000);
    //deep_sleep(5000);
    HAL_Delay(5000);
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
  ble_data.air_time = 100; // 50*100 = 5000ms 5 second
  // ble_data.env_temperature++;
  // ble_data.env_humidity++;
  // ble_data.dev_voltage++;

  // Send out a value
  send_ble_data(&hi2c1, &ble_data);

  //Sleep for air_time
  //half_sleep(ble_data.air_time * 100);
  HAL_Delay(ble_data.air_time * 100);

  //read scan data
  uint8_t received_data[2] = {0};
  do
  {
    //half_sleep(100);
    HAL_Delay(100);
    receive_ble_data(&hi2c1, received_data, 2);
  }
  while((received_data[0] + received_data[1]) != 255);
  //Make sure the received value is correct based on the second value

  //Set the value in our own data struct
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

  //Sleep for air_time
  //half_sleep(ble_data.air_time * 100);
  HAL_Delay(ble_data.air_time * 100);

  //read scan data
  uint8_t received_data[9+1] = {0};
  do
  {
    //half_sleep(100);
    HAL_delay(100);
    receive_ble_data(&hi2c1, received_data, 9+1);
  }
  while(received_data[0] + received_data[9] == 255);

  //If all are 0 except for received_data[9], then no beacon found

  ble_scan_data.ssr_id = received_data[0];
  ble_scan_data.temperature = received_data[1]>>8 | received_data[2];
  ble_scan_data.humidity = received_data[3];
  ble_scan_data.lux = received_data[4]>>8 | received_data[5];
  ble_scan_data.voltage = received_data[6]>>8 | received_data[7];
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