/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ltr_329.h"
#include "ble_module.h"
#include "sht40.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ble_module_data_t ble_data;
ble_beacon_result_t ble_beacon_result;
ble_scan_result_t ble_scan_result;
ssr_data_t ssr_data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* I2C lib function calls */
  bleDelayCallback(HAL_Delay);
  bleSendCallback(HAL_I2C_Master_Transmit);
  bleReceiveCallback(HAL_I2C_Master_Receive);
  bleAvailableCallback(HAL_I2C_IsDeviceReady);
  bleWakeCallback(wakeBleModule);

  /* ltr-386 lib function calls */
  ltrDelayCallback(HAL_Delay);
  // ltrWakeCallback(wakeltrModule);
  // ltrSleepCallback(sleepltrModule);

  /* sht40 lob function calls */
  sht40DelayCallback(HAL_Delay);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  int *debug_uart_buffer;
  debug_uart_buffer = (int *)malloc(40 * sizeof(char));

  while (1)
  {
    /* Read the ADC voltage */

    /* Determine path of execution based on voltage (energy) available */

    /* Initialize the lux sensor */
    ltr329Init(&hi2c1);

    /* Initialize the temperature and humidity sensor*/
    sht40Init(&hi2c1);

    /* Read out the light value */
    uint16_t lux = 0;
    float temperature = 0;
    float humidity = 0;

    ltr329GetLuxAll(&hi2c1, &lux);
    sht40ReadTempAndHumidity(&hi2c1, &temperature, &humidity, SHT40_HIGH_PRECISION); // highest precision.

    /* Sleep */
    ltr329Sleep(&hi2c1);
    sht40Sleep(&hi2c1);

    /* Compose data structure of the environment */
    ssr_data.env_humidity = (uint8_t)(humidity);
    ssr_data.env_temperature = (uint16_t)(temperature * 100);
    ssr_data.env_lux = (uint16_t)(lux);

    /* Display onto serial monitor */
    for(uint8_t i = 0; i < 30; i++) debug_uart_buffer[i]=32; //space character
    sprintf((char *)debug_uart_buffer, "lux: %d, t: %d, h: %d \r\n", ssr_data.env_lux, ssr_data.env_temperature, ssr_data.env_lux);
    HAL_UART_Transmit(&huart2, (uint8_t*)debug_uart_buffer, sizeof(debug_uart_buffer), 1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(2000);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void wakeBleModule()
{
  HAL_GPIO_WritePin(BLE_EN_GPIO_Port, BLE_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(BLE_EN_GPIO_Port, BLE_EN_Pin, GPIO_PIN_RESET);
}
void wakeltrModule()
{
  HAL_GPIO_WritePin(LTR329_EN_GPIO_Port, LTR329_EN_Pin, GPIO_PIN_SET);
}
void sleepltrModule()
{
  HAL_GPIO_WritePin(LTR329_EN_GPIO_Port, LTR329_EN_Pin, GPIO_PIN_RESET);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
