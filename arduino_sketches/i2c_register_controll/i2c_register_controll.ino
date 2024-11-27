#include <Wire.h>

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

uint8_t read_buffer[6];
uint8_t reg_val = 0x00;

void setGyroSampleRateDevider() {
  // Gyroscope sampling frequency 8kHz
  reg_val &= ~(0b11111111); // Clear all SMPLRT_DIV bits [7:0] - Lowest devider, highest rate
  //reg_val |=  (0b11111111); // Set devider to mode 4  - In between devider
  i2c_write(MPU6050_SMPLRT_DIV, reg_val);
}
void setGyroMeasureSpeed() {
  // Set gyroscope measuring speed
  reg_val &= ~(0b00111000);     // Clear all EXT_SYNC_SET bits [5:3]
  reg_val &= ~(0b00000111);     // Clear all DLPF_CFG bits [2:0] - Default mode 8kHz
  //reg_val |=  (0b00000001);     // Set output rate to mode 1  - Fastest on 1kHz
  //reg_val |=  (0b00000100);     // Set output rate to mode 6  - Slowest on 1kHz
  i2c_write(MPU6050_CONFIG, reg_val);
}
void setGyroMeasureAccuracy() {
  // Set gyroscope measuring accuracy
  reg_val &= ~(0b11100000);     // Clear all gyroscope test states [7:5]
  reg_val &= ~(0b00011000);     // Clear all FS_SEL bits [4:3] - Lowest accuracy
  //reg_val |=  (0b00011000);     // Set scale range to mode 3   - Highest accuracy
  i2c_write(MPU6050_GYRO_CONFIG, reg_val);
}
void setAccelMeasureAccuracy() {
  // Set accelerometer measuring accuracy
  reg_val &= ~(0b11100000);     // Clear all accelerometer test states [7:5]
  reg_val &= ~(0b00011000);     // Clear all AFS_SEL bits [4:3] - Lowest accuracy
  i2c_write(MPU6050_ACCEL_CONFIG, reg_val);
}
void setMotionTracking() {
  // Set motion tracking to least accurate
  reg_val |=  (0b11111111);     // Set all MOT_THR bits [7:0]
  i2c_write(MPU6050_MOT_THR, reg_val);
}
void setFifoBuffer() {
  //Set FIFO buffer access of sensors
  reg_val &= ~(0b10000000);     // Disable temperature sensor acces [7]
  reg_val |=  (0b01110000);     // Eneable gyroscope sensor acces [6:4]
  reg_val &= ~(0b00001000);     // Disable accelerometer sensor acces [3]
  reg_val &= ~(0b00000111);     // Disable I2C slave module access [2:0]
  i2c_write(MPU6050_FIFO_EN, reg_val);
}
void setIntPin() {
  // Disable interrupt on INT pin
  reg_val |= (0b11111110);      // Set all INT_PIN_CFG bits [7:1]
  i2c_write(MPU6050_INT_PIN_CONFIG, reg_val);
}
void setIntrupts() {
  // Disable all interrupts
  reg_val  &= ~(1 << 6);    // Disable accelerometer interrupt [6]
  reg_val  &= ~(1 << 4);    // Disable FIFO interrupt [4]
  reg_val  &= ~(1 << 3);    // Disable I2C master interrupt [3]
  reg_val  &= ~(1 << 0);    // Disable data available interrupt [0]
  i2c_write(MPU6050_INT_ENABLE, reg_val);
}
void setUserControl() {
  // Enable I2C & disable FIFO and master mode
  reg_val  &= ~(1 << 6);    // Disable FIFO buffer [6]
  reg_val  &= ~(1 << 5);    // Disable I2C master [5]
  reg_val  &= ~(1 << 4);    // Enable I2C interface [4]
  i2c_write(MPU6050_USER_CTRL, reg_val);
}
void setPowerManagement1() {
  // Power management 1
  reg_val &= ~(1 << 7);     // No device reset [7]
  reg_val &= ~(1 << 6);     // No sleep mode [6] <-----------------------
  reg_val &= ~(1 << 5);     // No timed measurements [5]
  reg_val |=  (1 << 3);     // Disable temperature sensor [3]
  reg_val &= ~(0b00000111); // Set clock reference to 8MHz [2:0]
  i2c_write(MPU6050_PWR_MGMT_1, reg_val);
}
void setPowerManagement2() {
  // Power management 2
  reg_val &= ~(0b11000000);     // Lowest wake up frequency (only accelerometer) [7:6]
  reg_val |=  (0b00111000);     // Disable accelerometer [5:3]
  reg_val &= ~(0b00000111);     // Enable gyroscope [2:0]
  i2c_write(MPU6050_PWR_MGMT_2, reg_val);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for Serial Monitor
  }

  Wire.begin(); // Initialize I2C

  setGyroSampleRateDevider(); // <--- Sample rate devider
  setGyroMeasureSpeed();      // <--- Output rate
  setGyroMeasureAccuracy();   // <--- Gyro accuracy
  setAccelMeasureAccuracy();
  setMotionTracking();
  setFifoBuffer();
  setIntPin();                // <--- Disable INT pin
  setIntrupts();              // <--- Disable interrupts
  setUserControl();           // <--- Disable FIFO and I2C master mode
  setPowerManagement1();      // <--- Sleep mode, disable temp sensor & clock reference
  setPowerManagement2();      // <--- Disable accelerometer

  // Verify communication by reading WHO_AM_I
  i2c_read(MPU6050_WHO_AM_I, read_buffer, 1);
  Serial.print("WHO_AM_I register: 0x");
  Serial.println(read_buffer[0], HEX);

  if (read_buffer[0] != 0x68) {
    Serial.println("Error: MPU6050 not detected. Check wiring!");
    while (1); // Halt if MPU6050 is not detected
  }

  Serial.println("MPU6050 initialized successfully!");
}

void loop() {
  // Read gyroscope data
  i2c_read(MPU6050_GYRO_OUT, read_buffer, 6);

  int16_t gyroX = (read_buffer[0] << 8) | read_buffer[1];
  int16_t gyroY = (read_buffer[2] << 8) | read_buffer[3];
  int16_t gyroZ = (read_buffer[4] << 8) | read_buffer[5];

  Serial.print("Gyro X: "); Serial.print(gyroX);
  Serial.print(" | Gyro Y: "); Serial.print(gyroY);
  Serial.print(" | Gyro Z: "); Serial.println(gyroZ);

  delay(1000);
}

// Function to write a single byte to a register
// void i2c_write(uint8_t reg_addr, uint8_t* buffer, size_t length) {
void i2c_write(uint8_t reg_addr, uint8_t settings) {
  Wire.beginTransmission(MPU6050_DEV_ADDR); // Start communication with the device
  Wire.write(reg_addr);                     // Send the register address
  Wire.write(settings);                     // Write each byte in the data buffer
  Wire.endTransmission();                   // End transmission and make the bus inactive
}

// Function to read multiple bytes starting from a register
void i2c_read(uint8_t reg_addr, uint8_t* buffer, size_t length) {
  Wire.beginTransmission(MPU6050_DEV_ADDR);
  Wire.write(reg_addr);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_DEV_ADDR, length);

  for (size_t i = 0; i < length; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    } else {
      buffer[i] = 0; // Fill with 0 if no data
    }
  }
}
