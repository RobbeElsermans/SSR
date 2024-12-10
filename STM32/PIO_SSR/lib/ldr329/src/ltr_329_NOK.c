#include "ltr_329_NOK.h"
#include "usart.h"

delay_callback_t _delay_callback;
void ltrDelayCallback(delay_callback_t dc_fp)
{
  _delay_callback = dc_fp;
}

// wake_device_t _wake_device;
// void ltrWakeCallback(wake_device_t wake_device_fp)
// {
//   _wake_device = wake_device_fp;
// }

// sleep_device_t _sleep_device;
// void ltrSleepCallback(sleep_device_t sleep_device_fp)
// {
//   _sleep_device = sleep_device_fp;
// }

// uint8_t device_active = 0;

void ltr329Init(I2C_HandleTypeDef* i2c_obj)
{
  // device_active = 1;
  // if(_wake_device) _wake_device();
  // _delay_callback(100); //initial wait time
  _delay_callback(200); // Initial startup time
  ltr329WakeUp(i2c_obj);

  uint8_t data[2];

  // Set ALS measurement rate and integration time
  data[0] = LTR329_ALS_MEAS_RATE;
  data[1] = 0x13; // 200ms integration, 500ms measurement rate
  HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);

  _delay_callback(100); // Initial startup time
}

HAL_StatusTypeDef ltr329ReadALS(I2C_HandleTypeDef* i2c_obj, uint8_t channel, uint16_t* lux)
{
  // if( !device_active )
  //   return 0; //Invalid

  uint8_t regAddr;
  uint8_t rawData0;
  uint8_t rawData1;

  // Select register address based on the channel
  if (channel == 0)
  {
    regAddr = LTR329_ALS_DATA_CH0_0;
  }
  else if (channel == 1)
  {
    regAddr = LTR329_ALS_DATA_CH1_0;
  }
  else
  {
    return 0; // Invalid channel
  }

  if (_delay_callback) _delay_callback(4000);

  // Read 2 bytes (low and high) from the sensor
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  if(ret != HAL_OK) return ret;

  //if (_delay_callback) _delay_callback(10);
  //if (_delay_callback) _delay_callback(4000);

  HAL_I2C_Master_Receive(i2c_obj, LTR329_I2C_ADDR, &rawData0, 1, HAL_MAX_DELAY);
  if(ret != HAL_OK) return ret;

  //if (_delay_callback) _delay_callback(10);
  //if (_delay_callback) _delay_callback(4000);

  regAddr++;
  HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  if(ret != HAL_OK) return ret;

  //if (_delay_callback) _delay_callback(10);
  //if (_delay_callback) _delay_callback(4000);

  HAL_I2C_Master_Receive(i2c_obj, LTR329_I2C_ADDR, &rawData1, 1, HAL_MAX_DELAY);
  if(ret != HAL_OK) return ret;

  uint16_t data = (rawData1 << 8) | rawData0; // Combine low and high bytes
  *lux = (rawData1 << 8) | rawData0; // Combine low and high bytes

  uint8_t Buffer[80] = {0};
  sprintf(Buffer, "taskSens - lux: %d\r\n", data);
  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 1000);
  //_delay_callback(8000);
  return ret;
}

HAL_StatusTypeDef ltr329GetLuxAll(I2C_HandleTypeDef* i2c_obj, uint16_t* lux)
{
    //Accordingly to datasheet, read them both.
    return ltr329ReadALS(i2c_obj, 1, lux); // Read channel 1 data
}

HAL_StatusTypeDef ltr329GetLuxIR(I2C_HandleTypeDef* i2c_obj, uint16_t* lux)
{
    //Accordingly to datasheet, read them both.
    return ltr329ReadALS(i2c_obj, 0, lux); // Read channel 0 data
}

//Should reach arround 5µA
HAL_StatusTypeDef ltr329Sleep(I2C_HandleTypeDef* hi2c1)
{
  uint8_t data[2];
  
  // Write to ALS_CONTR register to set the sensor in standby mode (0x00).
  data[0] = LTR329_ALS_CONTR;
  data[1] = 0x00; // Standby mode
  return HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef ltr329WakeUp(I2C_HandleTypeDef* hi2c1)
{
  uint8_t data[2];

  // Write to ALS_CONTR register to set the sensor in active mode.
  data[0] = LTR329_ALS_CONTR;
  data[1] = 0x01; // Active mode with default gain
  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);

  // Wait for the sensor to wake up.
  if (_delay_callback) _delay_callback(10); // According to the sensor datasheet
}

// void ltrSleep()
// {
//   // device_active = 0;
//   // _sleep_device();
// }