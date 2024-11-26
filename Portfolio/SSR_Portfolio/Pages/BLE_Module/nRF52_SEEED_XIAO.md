## BLE and STM32 I2C communication
To enable the BLE inter-communication, we use I2C as device connection to the STM32L4. 
Here, the BLE module shall have the address $0x12$ and the following struct will be send over.
```
struct ble_module_data_t
{
uint8_t mode; // The mode of the BLE-module, 0 -> beacon, 1-> scan
uint8_t ssr_id; // The ID of the rover itself
uint8_t air_time; // How long the beacon may last (val*100=ms)
int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
uint8_t env_humidity; // Range from -0-100%
uint16_t env_lux; // Range from 0 to 1000
uint16_t dev_voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
// x gyro 8bit?
// y gyro 8bit?
// z gyro 8bit?
};
```
A total of 11 bytes (without gyro or other parameters).

The STM32L4 will wake-up at a certain point and will check if the BLE-Beacon must be activated or not. If so, the BLE-module will be waken up on GPIO2 with a High flank trigger. This will bring the BLE-module out of deep sleep.

Then, STM32L4 will wait when the device is available on the I2C bus. After the BLE-module has set itself as available, the STM32L4 sends the *ble_module_data_t* struct with certain values over I2C to BLE-module.

Based on the *mode* it will either perform a beacon or a scanning operation for *air_time* amount of seconds.
