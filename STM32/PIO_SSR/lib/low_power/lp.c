#include "lp.h"
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

uint16_t counter_value(uint16_t time_millis)
{
  uint16_t counter_value = 0xFFFF; //= 0x2806; // 5 seconds
  // Calculate the time
  if (time_millis > 31981) //Maximum available wait time
    counter_value = 0xFFFF; // Set maximum available time
  else
    counter_value = (uint16_t)((time_millis / 1000.0) / 0.000488);

  if (counter_value <= 0x1)
  {
    counter_value = 0x2; //Correspondent to 0.976 ms
  }

  return counter_value;
}

void half_sleep(RTC_HandleTypeDef* hrtc, uint16_t time)
{
  //Setup RTC and setup interupt
  // MX_RTC_Init(); //DOne in main.c
  HAL_SuspendTick();
  HAL_RTCEx_SetWakeUpTimer_IT(hrtc, counter_value(time), RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);

  /* Enter STOP 2 mode */
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
  SystemClock_Config();
  HAL_ResumeTick();
}

void deep_sleep(RTC_HandleTypeDef* hrtc, uint16_t time)
{
  GPIO_Disable();
  HAL_PWREx_EnableSRAM2ContentRetention();
  HAL_PWREx_EnableBORPVD_ULP();
  HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
  HAL_RTCEx_SetWakeUpTimer_IT(hrtc, counter_value(time), RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);
  HAL_PWR_EnterSTANDBYMode();  
}