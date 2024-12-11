#include "gyro.h"

uint16_t delay_time_gyro = 1000;
delay_callback_t _delay_callback;
void gyroDelayCallback(delay_callback_t dc_fp) {
  _delay_callback = dc_fp;
}

void testMPU6050() {
  uint8_t who_am_i = 0;
  whoAmI(&who_am_i); //optional
  // char Buffer2[18] = {0};
  // sprintf(Buffer2, "Gyro Address: %02X\n", who_am_i);
  // serial_print(Buffer2);
  HAL_Delay(100);

  setMPU6050();
  HAL_Delay(100);

  uint16_t gryo_x, gryo_y, gryo_z;
  readGyroscope(&gryo_x, &gryo_y, &gryo_z);
  // char Buffer3[64] = {0};
  // sprintf(Buffer3, "Gyro X: %d | Gyro Y: %d | Gyro Z: %d\n", gryo_x, gryo_y, gryo_z);
  // serial_print(Buffer3);
  HAL_Delay(1000); // Delay for the next measurement
}

void whoAmI(uint8_t* who_am_i) {
  uint8_t data_tx[1] = {MPU6050_WHO_AM_I};

  // Who Am I test
  i2c_write_read(MPU6050_DEV_ADDR, data_tx, 1, who_am_i, 1);
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

void readGyroscope(uint16_t* gryo_x, uint16_t* gryo_y, uint16_t* gryo_z) {
    uint8_t data_tx[1] = { MPU6050_GYRO_OUT };
    uint8_t data_rx[6];

    i2c_write_read(MPU6050_DEV_ADDR, data_tx, 1, data_rx, 6);

    // Devide by 2 to send only 3 bytes instead of 6 (int16_t --> int8_t)
    // Highest value goes from 500 --> 250
    *gryo_x = (((data_rx[0] << 8) | data_rx[1])/131 >> 1); // gyro_x
    *gryo_y = (((data_rx[2] << 8) | data_rx[3])/131 >> 1); // gyro_y
    *gryo_z = (((data_rx[4] << 8) | data_rx[5])/131 >> 1); // gyro_z
}

void setGyroSleep(bool sleep) {
    uint8_t data_tx[2];
    data_tx[0] = MPU6050_PWR_MGMT_1;
    data_tx[1] = 0b01001011 & (sleep << 6);
    i2c_write(MPU6050_DEV_ADDR, data_tx, 2);
}
