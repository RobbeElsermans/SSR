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
#include "ble_module.h"
#include "ltr_329.h"
#include "linebot.h"
#include "gyro.h"
#include "sht4x.h"
#include "wio5e.h"

// #include "lp.h"
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

uint8_t bool_buffer = 0;

/* UART BUFFER */
uint8_t Buffer[140] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void clearBuf();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void){

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
  // ltrDelayCallback(HAL_Delay);
  
  // ltrWakeCallback(wakeltrModule);
  // ltrSleepCallback(sleepltrModule);

  /* sht40 lob function calls */
  // sht40DelayCallback(HAL_Delay);

  /* LineBot lob function calls */
  // lineBotDelayCallback(HAL_Delay);
  
  /* mpu6050 lob function calls */
  gyroDelayCallback(HAL_Delay);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* low power clock configuration */
  // lpClockConfigCallback(SystemClock_Config);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(2000);

  //I2C_Scan();

  char Buffer[11] = {0};
  sprintf(Buffer, "Her Am I\r\n");
  serial_print(Buffer);
  
  while(1) {
    test_code();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    taskReadBattery(); // Read the ADC voltage

    // Determine path of execution based on voltage (energy) available
    taskDetermineTasks();

    if (checkBool(&bool_buffer, TASK_SENS))
    {
      taskSens();                         // Do the task
      clearBool(&bool_buffer, TASK_SENS); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_STORE))
    {
      taskStore();                         // Do the task
      clearBool(&bool_buffer, TASK_STORE); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_LORA))
    {
      taskLora();                         // Do the task
      clearBool(&bool_buffer, TASK_LORA); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_SCAN))
    {
      taskScan();                         // Do the task
      clearBool(&bool_buffer, TASK_SCAN); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_BEACON))
    {
      taskBeacon();                         // Do the task
      clearBool(&bool_buffer, TASK_BEACON); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_DRIVE))
    {
      taskDrive();                         // Do the task
      clearBool(&bool_buffer, TASK_DRIVE); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_DEEP_SLEEP))
    {
      taskDeepSleep();                          // Do the task
      clearBool(&bool_buffer, TASK_DEEP_SLEEP); // Clear the bit in bool_buffer
    }

    if (checkBool(&bool_buffer, TASK_LIGHT_SLEEP))
    {
      taskLightSleep();                          // Do the task
      clearBool(&bool_buffer, TASK_LIGHT_SLEEP); // Clear the bit in bool_buffer
    }

    // Program shouldn't reach this place if deep sleep is used
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

void test_code() {
  char Buffer[16] = {0};
  sprintf(Buffer, "Test code\r\n");
  serial_print(Buffer);

  setupLoRa();
  ssr_data_t data;

  data.seq_number = 100;      // Range from 0 to 511 (8 bits total usage)
  data.env_temperature = 300; // Range from -327.68 to 327.67 °C (val/100=°C)
  data.env_humidity = 37;    // Range from -0-100%
  data.env_lux = 448;         // Range from 0 to 1000
  data.dev_voltage = 6245;     // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  data.gyro_x = 21;          // Range from -250 to 250 (val*2=°)
  data.gyro_y = 21;          // Range from -250 to 250 (val*2=°)
  data.gyro_z = 21;          // Range from -250 to 250 (val*2=°)

  send_data_over_lora(data);
}

void taskReadBattery()
{
  ssr_data.dev_voltage = (readVoltage(&hadc1))*1000.0; //COnvert to mV and save only int

  clearBuf();
  sprintf((char *)Buffer, "taskReadBattery - mV: %d \r\n", ssr_data.dev_voltage);
  serial_print(Buffer);
}

void taskDetermineTasks()
{
  // Do this based on the voltage value.

  if (ssr_data.dev_voltage >= 2.6) //Fully charged
  {
    bool_buffer = 0b10111111; //Do all tasks
  }
  else if (ssr_data.dev_voltage < 2.6 && ssr_data.dev_voltage >= 2.4)
  {
    bool_buffer = 0b10011111; //Do all tasks
  }
  else if (ssr_data.dev_voltage < 2.4 && ssr_data.dev_voltage >= 2.3)
  {
    
  }

  // Here, the boolean buffer **bool_buffer** is used with the defines of TASK described in main.h.
  // bool_buffer = 0b010000011; // Set DEEP_SLEEP, STORE, SENS,
  // bool_buffer = 0b10010001; // Set SLEEP, BEACON, SENS
  bool_buffer = 0b00000000; // Set SLEEP, SCAN, SENS
}

void taskSens()
{
  uint16_t lux = 0;
  float temperature = 0;
  float humidity = 0;

  /* Initialize sensors */
  HAL_Delay(1000);
  LTR329_Init(&hi2c1);
  // sht40Init(&hi2c1);

  HAL_Delay(1000);
  /* Read out the sens values */
  lux = GetLuxAll(&hi2c1);
  SHT40_ReadSensor(&temperature, &humidity);

  /* put to sleep/ halt */
  LTR329_Sleep(&hi2c1);
  SHT40_Sleep();

  /* Compose data structure of the environment */
  ssr_data.env_humidity = (uint8_t)(humidity);
  ssr_data.env_temperature = (uint16_t)(temperature * 100);
  ssr_data.env_lux = (uint16_t)(lux);

  /* Display onto serial monitor */
  clearBuf();
  sprintf(Buffer, "taskSens - lux: %d, t: %d, h: %d \r\n", ssr_data.env_lux, ssr_data.env_temperature, ssr_data.env_humidity);
<<<<<<< HEAD
  serial_print(Buffer);
=======
  serial_print(&Buffer);
>>>>>>> 1410043 (send over lora)
}

void taskStore()
{
  // TODO see Flash-Write project
}

void taskLora()
{
  // TODO
}

void taskScan()
{
  uint16_t air_time = 5000; // 5 seconds scanning

  ble_data.mode = 1;
  ble_data.ssr_id = SSR_ID;
  ble_data.air_time = air_time/100;

  ble_data.env_temperature = ssr_data.env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
  ble_data.env_humidity = ssr_data.env_humidity;       // Range from -0-100%
  ble_data.env_lux = ssr_data.env_lux;                 // Range from 0 to 1000
  ble_data.dev_voltage = ssr_data.dev_voltage;         // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  ble_data.dev_gyro_x = ssr_data.gyro_x;               // Range from -60 to 60 (val*3=°)
  ble_data.dev_gyro_y = ssr_data.gyro_y;               // Range from -60 to 60 (val*3=°)
  ble_data.dev_gyro_z = ssr_data.gyro_z;               // Range from -60 to 60 (val*3=°)

  clearBuf();
  sprintf((char *)Buffer, "taskScan - start scan %d \r\n", ble_data.air_time);
  serial_print(Buffer);

  ble_scan_result = scan(&hi2c1, &ble_data);

  /* Display onto serial monitor */
  clearBuf();
  sprintf((char *)Buffer,
          "taskScan - \r\n ssr_id: %d\r\n temp: %d\r\n h: %d\r\n l: %d\r\n x: %d\r\n y: %d\r\n z: %d\r\n vcc: %d\r\n rssi: %d\r\n",
          ble_scan_result.ssr_id, ble_scan_result.env_temperature, ble_scan_result.env_humidity, ble_scan_result.env_lux, ble_scan_result.dev_voltage, ble_scan_result.dev_gyro_x, ble_scan_result.dev_gyro_y, ble_scan_result.dev_gyro_z, ble_scan_result.rssi);
  serial_print(Buffer);
}

void taskBeacon()
{
  uint16_t air_time = 10000; // 10 seconds beacon

  ble_data.mode = 0;
  ble_data.ssr_id = SSR_ID;
  ble_data.air_time = air_time / 100;

  ble_data.env_temperature = ssr_data.env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
  ble_data.env_humidity = ssr_data.env_humidity;       // Range from -0-100%
  ble_data.env_lux = ssr_data.env_lux;                 // Range from 0 to 1000
  ble_data.dev_voltage = ssr_data.dev_voltage;         // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  ble_data.dev_gyro_x = ssr_data.gyro_x;               // Range from -60 to 60 (val*3=°)
  ble_data.dev_gyro_y = ssr_data.gyro_y;               // Range from -60 to 60 (val*3=°)
  ble_data.dev_gyro_z = ssr_data.gyro_z;               // Range from -60 to 60 (val*3=°)

  clearBuf();
  sprintf((char *)Buffer, "taskBeacon - start beacon %d \r\n", ble_data.air_time);
  serial_print(Buffer);

  ble_beacon_result = beacon(&hi2c1, &ble_data);

  /* Display onto serial monitor */

  clearBuf();
  sprintf((char *)Buffer, "taskBeacon - amount of ACK: %d \r\n", ble_beacon_result.amount_of_ack);
  serial_print(Buffer);
}

void taskDrive()
{
  // Do something with the struct *ble_scan_result* and gyro measurements + RSSI?
  // Do something with the struct *ble_beacon_result*?
  //  TODO
}

void taskDeepSleep()
{
  deep_sleep(&hrtc, 20000); // 20 seconds deep sleep
}

void taskLightSleep()
{
  half_sleep(&hrtc, 10000); // 20 seconds half sleep
}

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

uint16_t counter_value(uint16_t time_millis)
{
  uint16_t counter_value = 0xFFFF; //= 0x2806; // 5 seconds
  // Calculate the time
  if (time_millis > 31981)  // Maximum available wait time
    counter_value = 0xFFFF; // Set maximum available time
  else
    counter_value = (uint16_t)((time_millis / 1000.0) / 0.000488);

  if (counter_value <= 0x1)
  {
    counter_value = 0x2; // Correspondent to 0.976 ms
  }

  return counter_value;
}

void half_sleep(RTC_HandleTypeDef *hrtc, uint16_t time)
{
  // Setup RTC and setup interupt
  //  MX_RTC_Init(); //DOne in main.c
  HAL_SuspendTick();
  HAL_RTCEx_SetWakeUpTimer_IT(hrtc, counter_value(time), RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);

  /* Enter STOP 2 mode */
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
  SystemClock_Config();
  HAL_ResumeTick();
}

void deep_sleep(RTC_HandleTypeDef *hrtc, uint16_t time)
{
  GPIO_Disable();
  HAL_PWREx_EnableSRAM2ContentRetention();
  HAL_PWREx_EnableBORPVD_ULP();
  HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
  HAL_RTCEx_SetWakeUpTimer_IT(hrtc, counter_value(time), RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);
  HAL_PWR_EnterSTANDBYMode();
}

void GPIO_Disable(void)
{
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  // Set GPIO as analog read so floating.
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  /* Note: Debug using ST-Link is not possible during the execution of this   */
  /*       example because communication between ST-link and the device       */
  /*       under test is done through UART. All GPIO pins are disabled (set   */
  /*       to analog input mode) including  UART I/O pins.           */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
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

void clearBuf()
{
  for(uint8_t i = 0; i < sizeof(Buffer); i++)
  {
    Buffer[i] = 0;
  }
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
