#ifndef _GYRO_module_
#define _GYRO_module_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include "usart.h"
#include "i2c.h"

// MPU605 address
#define MPU6050_DEV_ADDR          0x68 // I2C address of the MPU6050

// MPU605 register addresses
#define MPU6050_SMPLRT_DIV        0x19 // sample rate divisor register
#define MPU6050_CONFIG            0x1A // General configuration register
#define MPU6050_GYRO_CONFIG       0x1B // Gyro specfic configuration register
#define MPU6050_ACCEL_CONFIG      0x1C // Accelerometer specific configration register
#define MPU6050_MOT_THR           0x1F // Motion detection threshold bits [7:0]

#define MPU6050_FIFO_EN           0x23 // Which sensor measurements are loaded into the FIFO buffer
#define MPU6050_INT_PIN_CONFIG    0x37 // Interrupt pin configuration register
#define MPU6050_INT_ENABLE        0x38 // Interrupt enable configuration register
#define MPU6050_GYRO_OUT          0x43 // Base address for gyroscope sensor data reads
#define MPU6050_USER_CTRL         0x6A // FIFO and I2C Master control register

#define MPU6050_PWR_MGMT_1        0x6B // Primary power/sleep control register
#define MPU6050_PWR_MGMT_2        0x6C // Secondary power/sleep control register
#define MPU6050_WHO_AM_I          0x75 // Divice ID register

// I2C methods
void i2c_write(uint8_t address, uint8_t* data_tx, uint8_t tx_size);
void i2c_write_read(uint8_t address, uint8_t* data_tx, uint8_t tx_size, uint8_t* data_rx, uint8_t rx_size);

// MPU6050 methods
void testMPU6050();
void setMPU6050();
void readGyroscope(uint16_t* gyro_data);
void setGyroSleep(bool sleep);

#ifdef __cplusplus
}
#endif

#endif