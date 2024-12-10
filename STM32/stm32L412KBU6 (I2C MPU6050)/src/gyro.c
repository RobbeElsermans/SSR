#include "gyro.h"

void i2c_write(uint8_t address, uint8_t* data_tx, uint8_t tx_size) {
  HAL_StatusTypeDef ret;

  ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
  if (ret != HAL_OK) {
    char Buffer[8] = {"Error\r\n"};
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
    HAL_Delay(1000);
  }
}

void i2c_write_read(uint8_t address, uint8_t* data_tx, uint8_t tx_size, uint8_t* data_rx, uint8_t rx_size) {
  HAL_StatusTypeDef ret;

  ret = HAL_I2C_Master_Transmit(&hi2c1, address << 1, data_tx, tx_size, 1000);
  if (ret != HAL_OK) {
    char Buffer[8] = {"Error\r\n"};
    HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
    HAL_Delay(1000);
  } else {
    // read bytes
    //HAL_Delay(1000);
    ret = HAL_I2C_Master_Receive(&hi2c1, address << 1, data_rx, rx_size, 1000);
    if (ret != HAL_OK) {
      char Buffer[8] = {"Error\r\n"};
      HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
      HAL_Delay(1000);
    } else {
      // for (int i = 0; i < rx_size; i++) {
      //   char Buffer[16] = {0};
      //   sprintf(Buffer, "data_rx[%i] = %02X\n", i, data_rx[i]);
      //   HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
      // }
    }
  }
}

void testMPU6050() {
  uint8_t data_tx[1] = { MPU6050_WHO_AM_I };
  uint8_t data_rx[1];

  // Who Am I test
  i2c_write_read(MPU6050_DEV_ADDR, data_tx, 1, data_rx, 1);
  char Buffer[17] = {0};
  sprintf(Buffer, "Gyro Address: %02X\n", data_rx[0]);
  HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), HAL_MAX_DELAY);
}

void setMPU6050() {
  uint8_t data_tx[2];

  // Gyroscope sampling frequency 8kHz
  data_tx[0] = MPU6050_SMPLRT_DIV;
  data_tx[1] = 0b00000000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Set gyroscope measuring speed
  data_tx[0] = MPU6050_CONFIG;
  data_tx[1] = 0b00000000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Set accelerometer measuring accuracy
  data_tx[0] = MPU6050_GYRO_CONFIG;
  data_tx[1] = 0b00000000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
  // Lowest accuracy means a division of 131
  // FS_SEL bits [4:3] --> 0: 131, 1: 65.5, 2: 32.8, 3: 16.4

  // Set motion tracking to least accurate
  data_tx[0] = MPU6050_MOT_THR;
  data_tx[1] = 0b11111111;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Set FIFO buffer access of sensors
  data_tx[0] = MPU6050_FIFO_EN;
  data_tx[1] = 0b01110000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Disable interrupt on INT pin
  data_tx[0] = MPU6050_INT_PIN_CONFIG;
  data_tx[1] = 0b11111110;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Disable all interrupts
  data_tx[0] = MPU6050_INT_ENABLE;
  data_tx[1] = 0b00000000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Enable I2C & disable FIFO and master mode
  data_tx[0] = MPU6050_USER_CTRL;
  data_tx[0] = 0b00000000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Power management 1
  data_tx[0] = MPU6050_PWR_MGMT_1;
  data_tx[1] = 0b00001011;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);

  // Power management 2
  data_tx[0] = MPU6050_PWR_MGMT_2;
  data_tx[1] = 0b00111000;
  i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
}

void readGyroscope(uint16_t* gyro_data) {
    uint8_t data_tx[1] = { MPU6050_GYRO_OUT };
    uint8_t data_rx[6];

    i2c_write_read(MPU6050_DEV_ADDR, data_tx, 1, data_rx, 6);

    gyro_data[0] = ((data_rx[0] << 8) | data_rx[1])/131; // gyro_x
    gyro_data[1] = ((data_rx[2] << 8) | data_rx[3])/131; // gyro_y
    gyro_data[2] = ((data_rx[4] << 8) | data_rx[5])/131; // gyro_z

    // Devide by 2 to send only 3 bytes instead of 6 (int16_t --> int8_t)
    // Highest value goes from 500 --> 250
    gyro_data[0] = (gyro_data[0] >> 1);
    gyro_data[1] = (gyro_data[1] >> 1);
    gyro_data[2] = (gyro_data[2] >> 1);

    HAL_Delay(1000); // Delay for the next measurement
}

void setGyroSleep(bool sleep) {
    uint8_t data_tx[2];
    data_tx[0] = MPU6050_PWR_MGMT_1;
    data_tx[1] = 0b01001011 & (sleep << 6);
    i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
}
