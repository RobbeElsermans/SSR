## BLE and STM32 I2C communication
![BLE code flow](/Images/BLE/BLE_Code_flow.png)

To enable the BLE inter-communication, we use I2C as device connection to the STM32L4. 
Here, the BLE module shall have the address $0x12$ and the following struct will be send over.
```c
struct ble_module_data_t
{
	uint8_t mode; // The mode of the BLE-module, 0 -> beacon, 1-> scan
	uint8_t ssr_id; // The ID of the rover itself
	uint8_t air_time; // How long the beacon may last (val*100=ms)
	int16_t env_temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
	uint8_t env_humidity; // Range from -0-100%
	uint16_t env_lux; // Range from 0 to 1000
	uint16_t dev_voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
	int8_t gyro_x; // Range from -60 to 60 (val*3=°)
	int8_t gyro_y; // Range from -60 to 60 (val*3=°)
	int8_t gyro_z; // Range from -60 to 60 (val*3=°)
};
```
A total of 13 bytes.

The STM32L4 periodically wakes up to check whether the BLE module needs to be activated. If activation is required, the BLE module is brought out of deep sleep by triggering a high signal on the GPIO2 pin.

After awakening the BLE module, the STM32L4 waits for it to become available on the I2C bus. Once the BLE module signals its availability, the STM32L4 sends a `ble_module_data_t` structure containing specific parameters to the BLE module via I2C.

Based on the provided `mode`, the BLE module will either perform a beacon operation or a scanning operation for a specified duration, defined by `air_time`.

Upon completing the operation, the BLE module waits for the STM32L4 to request the result. The result is either a `ble_beacon_result_t` or a `ble_scan_result_t`, depending on the mode selected at the start of the process.

*ble_beacon_result_t*  is selected when mode 0 or beacon mode was selected.
```c
struct ble_beacon_result_t
{
	uint8_t amount_of_ack;
};
```
This result will yield the amount of acknowledgements from scanners in the area.

The *ble_scan_result_t* gives a more detailed result of the found beacon in the area.
```c
struct ble_scan_result_t
{
	uint8_t ssr_id; // The ID of the source
	int16_t temperature; // Range from -327.68 to 327.67 °C (val/100=°C)
	uint8_t humidity; // Range from -0-100%
	uint16_t lux; // Range from 0 to 1000
	uint16_t voltage; // Range from 0-6.5535V (val/10000=V) (val/10=mV)
	int8_t rssi; //rssi
	int8_t gyro_x; // Range from -60 to 60 (val*3=°)
	int8_t gyro_y; // Range from -60 to 60 (val*3=°)
	int8_t gyro_z; // Range from -60 to 60 (val*3=°)
};
```
For now, only one beacon is saved and returned to the STM32L4 upon request. To validate the received data on the STM32L4 side, a small check is implemented. This check adds a byte to the return value, which is calculated as the difference of 255 and the first byte of the transmitted data.

For example, given the first byte `15` in the `ble_beacon_result_t` struct:

- The struct contains only one byte (`15`).
- An additional byte is added with the value `15 - 255 = 230`.
- The total bytes transmitted are `[15, 230]`.

Upon completing its task, the BLE module waits up to two seconds for a request from the STM32L4. If no request is received within this timeout, the BLE module discards the result to prevent unnecessary power consumption. This mechanism ensures that the BLE module does not remain in an indefinite waiting state.

Upon completing the operation, the BLE module waits for the STM32L4 to request the result. The result is either a `ble_beacon_result_t` or a `ble_scan_result_t`, depending on the mode selected at the start of the process.

![BLE scan response with emulated STM32L4](/Images/BLE/BLE_Scan_Response.png)

The following illustrates an example scenario:

- **ttyUSB0** (left): Represents the emulated STM32L4, which triggers the BLE module in mode 1 and waits for its response.
- **ttyACM2** (middle): Represents the BLE module in scanning mode (mode 1). It scans the medium until a beacon is discovered.
- **ttyACM3** (right): Represents the beacon BLE module connected to the STM32L4.

In this example, a beacon is fired, and the scanner detects it. The scanner saves the UUID values of the beacon with `SSR_id = 0x81`. This ID is visible in the data received by the emulated STM32L4. Other parameters include:

- Temperature: `0x71 = 113`
- Humidity: `0x00 = 0`
- Lux: `0x72 = 114`
- Voltage: `0x73 = 115`
- RSSI: `0xd4 = -44`.

## Modes
### Mode 0
Mode 0 or beacon mode will transmit a beacon. This beacon will be in air for *air_time* and will have a customised UUID of 16-bytes which is structured the following:
```
[0] ->  Temperature MSB(yte)
[1] ->  Temperature LSB(yte)
[2] ->  Humidity
[3] ->  Lux MSB(yte)
[4] ->  Lux LSB(yte)
[5] ->  Device Supercap Voltage MSB(yte)
[6] ->  Device Supercap Voltage LSB(yte)
[7] ->  gyro-x
[8] ->  gyro-y
[9] ->  gyro-z
[10] -> /0xFF
[11] -> /0xFF
[12] -> /0xFF
[13] -> /0xFF
[14] -> /0xFF
[15] -> /0xFF
```
The minor and major will have a certain purpose as well.
**Major**
- The major will consist of the upper byte as an ID of the BLE-device itself. This must be different for each BLE-device. Furthermore, a range of $[0-255]$ addresses can be used. 
  A total of *256* devices can exist at the same time for the same purpose.
- The lower byte of the major is the ID of the SSR-rover itself. This ID is defined in *ble_module_data_t* as *ssr_id*.
**Minor**
- The minor is not touched at the moment and is set to $0x0000$.
```
beacon.setMajorMinor((BEACON_SSR_ID << 8 |i2c_data.ssr_id), 0x0000);
```

When a scanner ACKs a beacon, the beacon gets halted for a moment. The beacon will count the amount of ACKs received. The amount of received ACKs will determine the amount of devices in present surrounding.

After the *air_time* has finished, the beacon will stop. The BLE module now waits for the STM32 to reached back. After the STM32 reached back or if the window has passed, the BLE goes to sleep again.

### Mode 1
Mode 1 is when we use the BLE module as scanner to search for BLE beacons in the proximity area. This allows for intercommunication between the SSRs. Same as Beacon, the scan will be in air for *air_time* and will have a named that represents the BLE-devide ID and the *ssr_id*.

All this data is encapsulated in the advertisement data packet. This consists of the following:
- Preamable (1-byte)
- Access Address (4-bytes)
- Protocol Data Unit (2-39-bytes)
- CRC (3-bytes)

![PDU Packet](/Images/BLE/BLE_LE_Packet.png)

Our wanted data will be situated in the Protocol Data Unit or PDU packet. Here we have the following structure:
- Header (2-bytes)
- Payload (0-37-bytes)

![PDU Packet](/Images/BLE/PDU_Packet.png)

And the header can be subdivided into:
- PDU types (4-bits)
- RFU (1-bit)
- ChSel (1-bit)
- TxAdd (1-bit)
- RxAdd (1-bit)
- Length (8-bit)

[reference for images of packet structures](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-2-bluetooth-le-advertising/topic/advertisement-packet/)

One note here, this is not a normal advertisement packet. This is an iBeacon packet which has a slightly different packet structure as described below.


An example where we distinguish all elements in the full Bluetooth BLE packet
```
02-01-06-1A-FF-59-00-02-15-00-5C-00-00-5D-00-5E-FF-FF-FF-FF-FF-FF-FF-FF-FF-3F-6C-00-00-CA

```

**1. Flags Field** (`02-01-06`):

- **Length (02)**: 2 bytes.
- **Type (01)**: Flags.
- **Value (06)**:
    - **LE General Discoverable Mode** (bit 1 set).
    - **BR/EDR Not Supported** (bit 2 set).

**2. Payload Specific Data** (`1A-FF-59-00-02-15-00-5C-00-00-5D-00-5E-FF-FF-FF-FF-FF-FF-FF-FF-FF-3F-6C-00-00-CA`):

- **Length (1A)**: 26 bytes.
- **Type (FF)**: Manufacturer Specific Data.
- **Company Identifier (59-00)**:
    - Assigned by Bluetooth SIG, corresponds to **Nordic Semiconductor ASA**.

**iBeacon-Specific Fields:**

- **Beacon Type (`02-15`)**:
    - `02`: iBeacon Indicator.
    - `15`: Indicates the iBeacon payload size (21 bytes).
- **UUID (`00-5C-00-00-5D-00-5E-FF-FF-FF-FF-FF-FF-FF-FF-FF`)**:
    - A unique identifier for this beacon.
    - This UUID encodes environmental telemetry, replacing a static UUID typical in iBeacon usage.
- **Major Value (`3F-6C`)**:
    - `3F` the id of a SSR beacon.
    - `6C` the SSR ID.
- **Minor Value (`00-00`)**:
    - not set in our project
- **Measured Power (`CA`)**:
    - The signal strength (RSSI) at 1 meter, used for distance estimation.
    - Decoded as `-54 dBm` (signed integer).

[Source](https://semiwiki.com/semiconductor-services/einfochips/302892-understanding-ble-beacons-and-their-applications/)

For now, we only save 1 beacon in our memory and we do not continue with scanning the medium. Nevertheless, the code is adapted to save multiple founded beacons within a specified *ari-time*.

After the *air_time* has finished or their was a beacon found, the scanner will stop. The BLE module now waits for the STM32 to reached back. After the STM32 reached back or if the window has passed, the BLE goes to sleep again.