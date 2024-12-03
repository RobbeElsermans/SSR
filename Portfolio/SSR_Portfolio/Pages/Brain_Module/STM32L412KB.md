## Code Flow
Our brain, STM32L412KB, will have a certain flow as depicted in the block schematic below.

![brain_flow_diagram](../../Images/brain_flow_diagram.pdf)

This is coded in a finite state machine which cycles through the different states. This to provide a minimal code footprint onto the MCU.


object of the rover to be used internally. This can be transmitted over LoRa for instance.
```
struct ble_module_data_t
{
uint8_t ssr_id; // The ID of the rover itself
int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
uint8_t env_humidity; // Range from -0-100%
uint16_t env_lux; // Range from 0 to 1000
uint16_t dev_voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
int8_t gyro_x; // Range from -60 to 60 (val*3=°)
int8_t gyro_y; // Range from -60 to 60 (val*3=°)
int8_t gyro_z; // Range from -60 to 60 (val*3=°)
};
```
