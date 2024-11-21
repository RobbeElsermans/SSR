BLE inter-communication
![[BLE-program_flow.pdf]]
A starting point for the BLE flow
I will describe here what I'll be using as raw advertisement or which services and characteristics I present with their UUID etc.

## BLE and STM32 I2C communication
To enable the BLE inter-communication, we use I2C as device connection to the STM32L4. 
Here, the BLE module shall have the address 0x10 and the following struct will be send over.
```
struct struct_BLE_object{
	uint8_t ssr_id; //The ID of the rover itself
	uint8_t beacon_time; //How long the beacon may last (val*100=ms)
	uint8_t scan_time; //How long the scan may last (val*100=ms)
	int16_t env_temperature; //Range from -327.68 to 327.67 °C (val/100=°C)
	uint8_t env_humidity; //Range from -0-100%
	uint16_t env_lux; //Range from 0 to 1000 
	uint16_t dev_voltage; //Range from 0-6.5535V (val/10000=V) (val/10=mV)
	//x gyro 8bit?
	//y gyro 8bit?
	//z gyro 8bit?
};
typedef structBLE_object BLE_object;
```
A total of 11 bytes.

The BLE-advertisement can hold 7-bytes where we select the *dev_voltage* (2-bytes), *env_temperature* (2-bytes), and *env_humidity* (1-byte) in the respectively order for now.

The BLE-object will be send over to the BLE module directly after. So we send a packet with 0x10 with the write bit enabled, and after that, we send 11 bytes.

When the BLE-module has finished its flow, it goes to deep sleep.
The SM32L4 needs to waken up the device by setting a specific GPIO HIGH.