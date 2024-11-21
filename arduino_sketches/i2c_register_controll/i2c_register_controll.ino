#include <Wire.h>

void i2c_write(uint8_t reg_addr, uint8_t* w_buf, size_t r_buf_size, bool stop = true);
void i2c_write_then_read(uint8_t reg_addr, uint8_t* w_buf, size_t r_buf_size, 
                         uint8_t* r_buf, size_t w_buf_size);
void i2c_write_then_read(uint8_t reg_addr, uint8_t* r_buf, size_t w_buf_size);

#define MPU6050_DEV_ADDR          0x68 // I2C address of the MPU6050

#define MPU6050_SELF_TEST_X       0x0D // Self test factory calibrated values register
#define MPU6050_SELF_TEST_Y       0x0E // Self test factory calibrated values register
#define MPU6050_SELF_TEST_Z       0x0F // Self test factory calibrated values register
#define MPU6050_SELF_TEST_A       0x10 // Self test factory calibrated values register

#define MPU6050_SMPLRT_DIV        0x19 // sample rate divisor register
#define MPU6050_CONFIG            0x1A // General configuration register
#define MPU6050_GYRO_CONFIG       0x1B // Gyro specfic configuration register
#define MPU6050_ACCEL_CONFIG      0x1C // Accelerometer specific configration register
#define MPU6050_MOT_THR           0x1F // Motion detection threshold bits [7:0]
#define MPU6050_FIFO_EN           0x23 // Which sensor measurements are loaded into the FIFO buffer

// i2c registers
// 0x24 - 0x36

#define MPU6050_INT_PIN_CONFIG    0x37 // Interrupt pin configuration register
#define MPU6050_INT_ENABLE        0x38 // Interrupt enable configuration register
#define MPU6050_INT_STATUS        0x3A // Interrupt status register

//#define MPU6050_ACCEL_OUT         0x3B // Base address for accelerometer sensor data reads
//#define MPU6050_TEMP_H            0x41 // Temperature data high byte register
//#define MPU6050_TEMP_L            0x42 // Temperature data low byte register
#define MPU6050_GYRO_OUT          0x43 // Base address for gyroscope sensor data reads

// ext sensor data register (slave i2c)
// 0x49 - 0x60 & 0x63 - 0x67

#define MPU6050_SIGNAL_PATH_RESET 0x68 // Signal path reset register
#define MPU6050_MOT_DETECT_CTRL   0x69 // Delay for the accelerometer power on time
#define MPU6050_USER_CTRL         0x6A // FIFO and I2C Master control register
#define MPU6050_PWR_MGMT_1        0x6B // Primary power/sleep control register
#define MPU6050_PWR_MGMT_2        0x6C // Secondary power/sleep control register

// FIFO registers
// 0x72 - 0x74

#define MPU6050_WHO_AM_I          0x75 // Divice ID register

#define write_buf_size 16
#define read_buf_size 16

uint8_t write_buffer[write_buf_size];
uint8_t read_buffer[read_buf_size];

uint8_t reg_val = 0x00;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for Serial Monitor to connect
  }

  Wire.begin(); // Start I2C communication
  
  // Gyroscope sampling frequency 8kHz
  reg_val &= ~(0xFF);     // Clear all SMPLRT_DIV bits [7-0] - Lowest devider, highest rate
  //reg_val |= (0x0F);      // Set devider to mode 4  - In between devider
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_SMPLRT_DIV, write_buffer, 1);

  // Set gyroscope measuring speed
  reg_val &= ~(0x38);     // Clear all EXT_SYNC_SET bits [5-3]
  reg_val &= ~(0x07);     // Clear all DLPF_CFG bits [2-0] - Default mode 8kHz
  //reg_val |= (0x01);      // Set output rate to mode 1  - Fastest on 1kHz
  //reg_val |= (0x04);      // Set output rate to mode 6  - Slowest on 1kHz
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_CONFIG, write_buffer, 1);

  // Set gyroscope measuring accuracy
  reg_val &= ~(0xE0);     // Clear all gyroscope test states [7-5]
  reg_val &= ~(0x18);     // Clear all FS_SEL bits [2-0] - Lowest accuracy
  //reg_val |= (0x18);      // Set scale range to mode 3   - Highest accuracy
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_GYRO_CONFIG, write_buffer, 1);

  // Set accelerometer measuring accuracy
  reg_val &= ~(0xE0);     // Clear all accelerometer test states [7-5]
  reg_val &= ~(0x18);     // Clear all AFS_SEL bits [2-0] - Lowest accuracy
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_ACCEL_CONFIG, write_buffer, 1);

  // Set motion tracking to least accurate
  reg_val |= (0xFF);      // Set all MOT_THR bits [7-5]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_MOT_THR, write_buffer, 1);

  /*// Set FIFO buffer access of sensors
  reg_val &= ~(0x80);     // Disable temperature sensor acces [7]
  reg_val |= (0x70);      // Eneable gyroscope sensor acces [6-4]
  reg_val &= ~(0x08);     // Disable accelerometer sensor acces [3]
  reg_val &= ~(0x07);     // Disable I2C slave module access [2-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_FIFO_EN!, write_buffer, 1);*/

  // Disable interrupt on INT pin
  reg_val |= (0xFF);      // Set all INT_PIN_CFG bits [7-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_INT_PIN_CONFIG, write_buffer, 1);

  // Disable all interrupts
  reg_val  &= ~(0xFF);    // Disable all interrupts [7-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_INT_ENABLE, write_buffer, 1);

  // Read out gyroscope value
  i2c_write_then_read(MPU6050_GYRO_OUT, read_buffer, 6);
  
  i2c_write_then_read(MPU6050_GYRO_OUT, read_buffer, 1);    // X high
  i2c_write_then_read(MPU6050_GYRO_OUT+1, read_buffer, 1);  // X low
  i2c_write_then_read(MPU6050_GYRO_OUT+2, read_buffer, 1);  // Y high
  i2c_write_then_read(MPU6050_GYRO_OUT+3, read_buffer, 1);  // Y low
  i2c_write_then_read(MPU6050_GYRO_OUT+4, read_buffer, 1);  // Z high
  i2c_write_then_read(MPU6050_GYRO_OUT+5, read_buffer, 1);  // Z low

  // Enable I2C & disable FIFO and master mode
  reg_val &= ~(0xFF);      // Clear all USER_CTRL bits [7-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_USER_CTRL, write_buffer, 1);
  
  // Power management
  reg_val &= ~(0x80);     // No device reset [7]
  reg_val &= ~(0x40);     // No sleep mode [6] <-----------------------
  reg_val &= ~(0x20);     // No timed measurements [5]
  reg_val |= (0x20);      // Disable temperature sensor [3]
  reg_val &= ~(0x07);     // Set clock reference to 8MHz [2-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_USER_CTRL, write_buffer, 1);
  reg_val &= ~(0xC0);     // Wake up frequency (only accelerometer) [7-6]
  reg_val |= (0x38);      // Disable accelerometer [5-3]
  reg_val &= ~(0x07);     // Enable gyroscope [2-0]
  write_buffer[0] = {reg_val};
  i2c_write(MPU6050_USER_CTRL, write_buffer, 1);

  // Test module
  // communications_test();    // Test write and read functions
  // perform_inegrity_check(); // Check gyroscope (X-axis)
}

void loop() {
  // Empty loop
}

void communications_test() {
  i2c_write_then_read(MPU6050_WHO_AM_I, read_buffer, 1);
}
void perform_inegrity_check() {
  // Activate self-test for Gyroscope X-axis
  write_buffer[0] = {0xE0};
  i2c_write(MPU6050_GYRO_CONFIG, write_buffer, 1);
  // Read the SELF_TEST_X register
  i2c_write_then_read(MPU6050_SELF_TEST_X, read_buffer, 1);
  // Reset GYRO_CONFIG to normal operation
  write_buffer[0] = {0x00};
  i2c_write(MPU6050_GYRO_CONFIG, write_buffer, 1);
}

void i2c_write(uint8_t reg_addr, uint8_t* w_buf, size_t r_buf_size, bool stop = true) {
  Wire.beginTransmission(MPU6050_DEV_ADDR); // Start communication with the device
  Wire.write(reg_addr);                     // Send the register address
  for (size_t i = 0; i < r_buf_size; i++) { // Write each byte in the data buffer
    Wire.write(w_buf[i]);
  }
  Wire.endTransmission(stop);               // End transmission and make the bus inactive
}
void i2c_write_then_read(uint8_t reg_addr, uint8_t* w_buf, size_t r_buf_size, 
                         uint8_t* r_buf, size_t w_buf_size) {
  // Write the register address we want to read
  i2c_write(reg_addr, w_buf, r_buf_size, false);

  // Request multiple bytes from the device
  Wire.requestFrom(MPU6050_DEV_ADDR, w_buf_size);
  
  uint8_t index = 0;
  while (Wire.available() && index < w_buf_size) {
    r_buf[index++] = Wire.read(); // Read each byte into the buffer
  }

  // Debug output for the received data
  Serial.print("Data read from device: ");
  for (uint8_t i = 0; i < w_buf_size; i++) {
    Serial.print("0x");
    Serial.print(r_buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
void i2c_write_then_read(uint8_t reg_addr, uint8_t* r_buf, size_t w_buf_size) {
  i2c_write_then_read(reg_addr, {0}, 0, r_buf, w_buf_size);
}
