#include "ltr_329.h"

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

void LTR329_Init(I2C_HandleTypeDef* i2c_obj)
{
  // device_active = 1;
  // if(_wake_device) _wake_device();
  // _delay_callback(100); //initial wait time
  LTR329_WakeUp(i2c_obj);

  uint8_t data[2];

  // Set ALS measurement rate and integration time
  data[0] = LTR329_ALS_MEAS_RATE;
  data[1] = 0x13; // 200ms integration, 500ms measurement rate
  HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
  _delay_callback(100); // Initial startup time
}

uint16_t LTR329_ReadALS(I2C_HandleTypeDef* i2c_obj, uint8_t channel)
{
  // if( !device_active )
  //   return 0; //Invalid

  uint8_t regAddr;
  uint8_t rawData[2];
  uint16_t data;

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

  // Read 2 bytes (low and high) from the sensor

  HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  //HAL_Delay(10);
  if (_delay_callback) _delay_callback(10);
  HAL_I2C_Master_Receive(i2c_obj, LTR329_I2C_ADDR, &rawData[0], 1, HAL_MAX_DELAY);
  //HAL_Delay(10);
  if (_delay_callback) _delay_callback(10);
  regAddr++;
  HAL_I2C_Master_Transmit(i2c_obj, LTR329_I2C_ADDR, &regAddr, 1, HAL_MAX_DELAY);
  //HAL_Delay(10);
  if (_delay_callback) _delay_callback(10);
  HAL_I2C_Master_Receive(i2c_obj, LTR329_I2C_ADDR, &rawData[1], 1, HAL_MAX_DELAY);

  data = (rawData[1] << 8) | rawData[0]; // Combine low and high bytes
  return data;
}

uint16_t LTR329_GetLuxAll(I2C_HandleTypeDef* i2c_obj)
{
    //Accordingly to datasheet, read them both.
    uint16_t ch1 = LTR329_ReadALS(i2c_obj, 1); // Read channel 1 data

    return ch1;
}

uint16_t LTR329_GetLuxIR(I2C_HandleTypeDef* i2c_obj)
{
    //Accordingly to datasheet, read them both.
    uint16_t ch0 = LTR329_ReadALS(i2c_obj, 0); // Read channel 0 data

    return ch0;
}

//Should reach arround 5µA
void LTR329_Sleep(I2C_HandleTypeDef* hi2c1)
{
  uint8_t data[2];
  
  // Write to ALS_CONTR register to set the sensor in standby mode (0x00).
  data[0] = LTR329_ALS_CONTR;
  data[1] = 0x00; // Standby mode
  HAL_I2C_Master_Transmit(hi2c1, LTR329_I2C_ADDR, data, 2, HAL_MAX_DELAY);
}
void LTR329_WakeUp(I2C_HandleTypeDef* hi2c1)
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