## BLE and STM32 I2C communication
![BLE_block_diagram](../../Images/Block_Diagram_BLE.png)
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
[7] ->  /0xFF
[8] ->  /0xFF
[9] ->  /0xFF
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
- The minor is not touched at the moment and is set to $0x00000$.

When a scanner ACKs a beacon, the beacon gets halted for a moment. The beacon will count the amount of ACKs received. The amount of received ACKs will determine the amount of devices in present surrounding.

After the *air_time* has finished, the beacon will stop. The BLE module now waits for the STM32 to reached back. After the STM32 reached back, the BLE goes to sleep again.

### Mode 1
Mode 1 is when we use the BLE module as scanner to search for BLE beacons in the proximity area. This allows for intercommunication between the SSRs. Same as Beacon, the scan will be in air for *air_time* and will have a named that represents the BLE-devide ID and the *ssr_id*.

All this data is encapsulated in the advertisement data packet. This consists of the following:
- Preamable (1-byte)
- Access Address (4-bytes)
- Protocol Data Unit (2-39-bytes)
- CRC (3-bytes)

![PDU Packet](BLE_LE_Packet.png)

Our wanted data will be situated in the Protocol Data Unit or PDU packet. Here we have the following structure:
- Header (2-bytes)
- Payload (0-37-bytes)

![PDU Packet](PDU_Packet.png)

And the header can be subdivided into:
- PDU types (4-bits)
- RFU (1-bit)
- ChSel (1-bit)
- TxAdd (1-bit)
- RxAdd (1-bit)
- Length (8-bit)

![Header Packet](../../Images/Header_Packet.png)

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

**2. Manufacturer Specific Data** (`1A-FF-59-00-02-15-00-5C-00-00-5D-00-5E-FF-FF-FF-FF-FF-FF-FF-FF-FF-3F-6C-00-00-CA`):

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