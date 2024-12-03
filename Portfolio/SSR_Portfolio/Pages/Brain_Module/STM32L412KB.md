## Code Flow
Our brain, STM32L412KB, will have a certain flow as depicted in the block schematic below.

![brain_flow_diagram](../../Images/brain_flow_diagram.pdf)

This is coded in a finite state machine which cycles through the different states. This to provide a minimal code footprint onto the MCU.

```
struct ble_module_data_t
{
uint8_t ssr_id; // The ID of the rover itself
int32_t counter; //The packet counter of the measurements.
int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
uint8_t env_humidity; // Range from -0-100%
uint16_t env_lux; // Range from 0 to 1000
uint16_t dev_voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
int8_t gyro_x; // Range from -60 to 60 (val*3=°)
int8_t gyro_y; // Range from -60 to 60 (val*3=°)
int8_t gyro_z; // Range from -60 to 60 (val*3=°)
};
```

## EEPROM (or flash)
To enable a measurement and keep the value in our system when we enter a deep sleep mode, we need a certain storage for these values. This MCU has a flash region up to 128KB flash. 

The default range of the flash in this MCU starts from $0x0800 0000$ and ends on $0x0802 0000$.
This gives us a total range of $0x20000$. Furthermore, this includes the MCUs code. Therefore, in order to write into the flash, it is mandatory to not tamper with the code of the program and build in some kind of failsafe so we do not write users code.

That is why the EEPROM will be used starting from address $0x0801E000$ and ending until $0x0802 0000$. This gives us 8KBytes to work with. Or to give some reference to the struct we will send over, $8000/15 =533.3 \approx 530$. We can store 530 messages before we filled up the 8KBytes.




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
