#include "lp.h"

/* RTC handler declaration */
RTC_HandleTypeDef RTCHandle;
static __IO uint32_t TimingDelay;

uint8_t RTC_Config(void)
{
    /* Configure RTC */
    RTCHandle.Instance = RTC;
    /* Set the RTC time base to 1s */
    /* Configure RTC prescaler and RTC data registers as follow:
      - Hour Format = Format 24
      - Asynch Prediv = Value according to source clock
      - Synch Prediv = Value according to source clock
      - OutPut = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType = Open Drain */
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
        /* Initialization Error */
        return 1;
    }

    /* Set RTC calibration low power mode */
    HAL_RTCEx_SetLowPowerCalib(&RTCHandle, RTC_LPCAL_SET);
    return 0;
}

uint8_t lowPower_init(void)
{
    uint32_t tickstart;
    /* Configure the system Power */
    GPIO_Disable();
    /* Enable ultra low power BOR and PVD supply monitoring */
    HAL_PWREx_EnableBORPVD_ULP();

    /* The Following Wakeup sequence is highly recommended prior to each Standby mode entry
       mainly when using more than one wakeup source this is to not miss any wakeup event.
       - Disable all used wakeup sources,
       - Clear all related wakeup flags,
       - Re-enable all used wakeup sources,
       - Enter the Standby mode.
    */
    /* Disable all used wakeup sources */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Re-enable wakeup source */
    /* ## Setting the Wake up time ############################################*/
    /* RTC Wakeup Interrupt Generation:
      the wake-up counter is set to its maximum value to yield the longest
      stand-by time to let the current reach its lowest operating point.
      The maximum value is 0xFFFF, corresponding to about 33 sec. when
      RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 */

    /* Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE))
       Wakeup Time = Wakeup Time Base * WakeUpCounter
         = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE)) * WakeUpCounter
         ==> WakeUpCounter = Wakeup Time / Wakeup Time Base   */

    /* To configure the wake up timer to 33s the WakeUpCounter is set to 0xFFFF:
       Wakeup Time Base = 16 /(~32 kHz RC) = ~0.5 ms
       Wakeup Time = 0.5 ms  * WakeUpCounter
       Therefore, with wake-up counter =  0xFFFF  = 65,535
         Wakeup Time =  0.5 ms *  65,535 = ~ 33 sec. */
    //HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0x8FFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0);

    /* Need to wait for 2 RTC clock cycles before entering Standby mode when RTC is clocked by LSE,
       so wait for RSF flag to ensure the delay is satisfied */
    tickstart = HAL_GetTick();
    while ((RTCHandle.Instance->ICSR & RTC_ICSR_RSF) == 0U)
    {
        if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
        {
            return 1;
        }
    }
    return 0;
}

/**
  * @brief  System Power Configuration
  *         The system Power is configured as follow:
  *            + Automatic Wakeup using RTC clocked by LSI
  * @param  None
  * @retval None
  */
void GPIO_Disable(void)
{
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  //Set GPIO as analog read so floating.
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