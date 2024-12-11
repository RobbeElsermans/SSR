#include <Wire.h>

uint8_t address = 0x68; // I2C address of the MPU6050

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for Serial Monitor to connect
  }

  uint8_t WHO_AM_I = 0x75; // Register address for WHO_AM_I
  uint8_t read_buffer = 0;

  Wire.begin(); // Initialize I2C communication

  // Write the register address we want to read
  Wire.beginTransmission(address);
  Wire.write(WHO_AM_I); // Send register address
  Wire.endTransmission(false); // End transmission but keep the bus active

  // Request 1 byte from the device
  Wire.requestFrom(address, (uint8_t)1);
  while (Wire.available()) {
    read_buffer = Wire.read(); // Read the received byte
  }
  

  Serial.print("WHO_AM_I register: 0x");
  Serial.println(read_buffer, HEX); // Print the value in hexadecimal
}

void loop() {
  // Empty loop
}
