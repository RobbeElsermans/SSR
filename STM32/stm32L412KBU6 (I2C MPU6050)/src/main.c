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

/** @addtogroup STM32L4xx_HAL_Examples
 * @{
 */

/** @addtogroup PWR_STANDBY_RTC
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t BLE_Counter = 0;
uint8_t buf[10]; // I2C recieve buffer

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
  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 80 MHz */
  SystemClock_Config();

  // init pheripherals
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_UART2_UART_Init();
  LTR329_Init(&hi2c1); // Initialize LTR-329 sensor

  /* Configure RTC */
  if (RTC_Config()) { Error_Handler(); }

  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

  /* Check if the system was resumed from StandBy mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }

  /* Insert 5 seconds delay */
  //HAL_Delay(5000);

  /* Enter the Standby mode */
  // if (lowPower_init())
  // {
  //   Error_Handler();
  // }
  // else
  // {
  //   HAL_PWR_EnterSTANDBYMode();
  // }

  /* Program should never reach this point (program restart when exiting from standby mode) */
  // Error_Handler();

  /* Use to find I2C addresses on the bus */
  // I2C_scan();

  setMPU6050();
  uint16_t gyro_data[3];

  while (1) {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    
    readGyroscope(gyro_data);
    char Buffer[64] = {0};
    sprintf(Buffer, "Gyro X: %d | Gyro Y: %d | Gyro Z: %d\n", gyro_data[0], gyro_data[1], gyro_data[2]);
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);

    HAL_Delay(1000); // Delay for the next measurement
  }
}

// void setMPU6050() {
//   uint8_t data_tx[2];
//   uint8_t data_rx[6];

//   // Who Am I test
//   data_tx[0] = MPU6050_WHO_AM_I;
//   i2c_write_read(MPU6050_DEV_ADDR, data_tx, 1, data_rx, 1);

//   // Gyroscope sampling frequency 8kHz
//   data_tx[0] = MPU6050_SMPLRT_DIV;
//   data_tx[1] = 0b00000000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Set gyroscope measuring speed
//   data_tx[0] = MPU6050_CONFIG;
//   data_tx[1] = 0b00000000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Set accelerometer measuring accuracy
//   data_tx[0] = MPU6050_GYRO_CONFIG;
//   data_tx[1] = 0b00000000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
//   // Lowest accuracy means a division of 131
//   // FS_SEL bits [4:3] --> 0: 131, 1: 65.5, 2: 32.8, 3: 16.4

//   // Set motion tracking to least accurate
//   data_tx[0] = MPU6050_MOT_THR;
//   data_tx[1] = 0b11111111;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Set FIFO buffer access of sensors
//   data_tx[0] = MPU6050_FIFO_EN;
//   data_tx[1] = 0b01110000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Disable interrupt on INT pin
//   data_tx[0] = MPU6050_INT_PIN_CONFIG;
//   data_tx[1] = 0b11111110;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Disable all interrupts
//   data_tx[0] = MPU6050_INT_ENABLE;
//   data_tx[1] = 0b00000000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Enable I2C & disable FIFO and master mode
//   data_tx[0] = MPU6050_USER_CTRL;
//   data_tx[0] = 0b00000000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Power management 1
//   data_tx[0] = MPU6050_PWR_MGMT_1;
//   data_tx[1] = 0b00001011;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

//   // Power management 2
//   data_tx[0] = MPU6050_PWR_MGMT_2;
//   data_tx[1] = 0b00111000;
//   i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
// }

// void i2c_write(uint8_t address, uint8_t* data_tx, uint8_t tx_size) {
//   HAL_StatusTypeDef ret;

//   ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
//   if (ret != HAL_OK) {
//     char Buffer[8] = {"Error\r\n"};
//     HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
//     HAL_Delay(1000);
//   }
// }

// void i2c_write_read(uint8_t address, uint8_t* data_tx, uint8_t tx_size, uint8_t* data_rx, uint8_t rx_size) {
//   HAL_StatusTypeDef ret;

//   ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
//   if (ret != HAL_OK) {
//     char Buffer[8] = {"Error\r\n"};
//     HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
//     HAL_Delay(1000);
//   } else {
//     // read bytes
//     //HAL_Delay(1000);
//     ret = HAL_I2C_Master_Receive(&hi2c1, address << 1, data_rx, rx_size, 1000);
//     if (ret != HAL_OK) {
//       char Buffer[8] = {"Error\r\n"};
//       HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
//       HAL_Delay(1000);
//     } else {
//       for (int i = 0; i < rx_size; i++) {
//         char Buffer[16] = {0};
//         sprintf(Buffer, "data_rx[%i] = %02X\n", i, data_rx[i]);
//         HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
//       }
//     }
//   }
// }

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

/**
 * @}
 */

/**
 * @}
 */