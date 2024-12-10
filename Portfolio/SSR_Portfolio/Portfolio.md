# Table of contents
==To Be Generated At the end==

# SSR
By Adam Hejduk, Robbe Elsermans, and Thomas Kramp
## Intro
As humanities interest in Mars grows, an understanding of its weather is needed. One of the greatest challenges that the planet provides are the frequent dust storms.
This project will try to monitor the weather phenomenon (such as temperature, humidity, light, ...) to predict such storms.
## Project goal
1 systems that is his own master. They wonder around on the surface of mars. When a node gets lost or anything else, and another node approaches, it can share valuable information. For the rest, they act on their own.
BLE is used to check proximities of each other and share some valuable information that could have been lost during dust storms (due to no communication available). Also, if a device is unable to move due to a mechanical problem, it can still transmit its data to surrounding nodes.
## Project division
#### to divide

#### Adam
- Environmental sensing
- Energy harvesting
#### Thomas
- LoRa connectivity
- Gyroscope
- Rover actuation
#### Robbe
- Project management
- BLE communication
- Power Profiling
- Energy awareness algorithm
## Board selection
[Brain Board Selection](Pages/Investigation/Brain_Board_Selection.md)


## MVP
The MVP (Minimal Vital Product) contains the following items:
- LoRa-module that transmits data to a gateway.
- Dashboard to review received data from LoRa-module.
- The ability to measure temperature and humidity.
- Energy awareness
- Use environmental source as energy source
## Power Profiling
[Power Profiling](Pages/Power_Profiling/Power_Profiling.md)
## STM32 Module
[STM32 module](Pages/Brain_module/STM32L412KB.md)
## BLE Module code
[BLE Module](Pages/BLE_Module/nRF52_SEEED_XIAO.md)
## I2C communication sensor SHT40
[SHT40](Pages/Sensor/SHT40.md)
- addressing an array x addressing a place in an array 
- UART printing is not the same as printf sprintf...
- HAL libary learining
- working with datasheets of more complex nature for the first time 
- programing in more dvanced C


## I2C communication of LTR-329
[LTR-329](Pages/Sensor/LTR-329.md)


Sleepmode: 
### . **How It Works**

1. **Active State**:
    
    - The sensor reads ambient light data and transmits it via UART.
    - The LED (`LD3_Pin`) is turned on to indicate the active state.
2. **Sleep State**:
    
    - The `LTR329_Sleep` function puts the sensor into standby mode.
    - The LED is turned off to indicate the sleep state.
    - The system waits for 5 seconds (or your preferred duration).
3. **Wake-Up State**:
    
    - The `LTR329_WakeUp` function sets the sensor back to active mode.
    - A delay is added after wake-up to allow the sensor to stabilize before the next reading.
![[Pasted image 20241203122154.png]] 
according to datasheet: 
standby current: 5 $micro$A
initial startup: 100 ms
waking up: 10 ms 
## Energy Harvesting
[Enegry Harvesting Module AEM1094](AEM10941.md)
The goal is to pover as many peripherals as possible... SHT40, LTR-329, STM32, BLE ?
We have:
- Outside S.P.
- Inside S.P.
- 2x 2,7V1F Supercap
- 2x AEM10941 Evaluation Kit 
Note to self: Make sure to have SuperCap connected before connecting power source (S.P.)

Tested Single Cell Configuration: It seems to be ok on 2,7V treshhold, the module gets slighty over 2,7 Volts and than regulates it down so that the Capacitor is okay. 
HighOUT: 1,8V
LowOUT? 1,2V
according to datasheet
It takes about 1h 35m to charge the Cap. to 2,3V WITH additional artiffical lights
![[Pasted image 20241129210421.png]]

### Voltage measurement of the energy harvesting module
Hardware Setup: 
- Use the **ADC (Analog-to-Digital Converter)** on the STM32 to measure the voltage of the energy storage device.
- Connect the energy storage's (SUPERCAP) positive terminal to a voltage divider to scale it down if it exceeds the ADC input range of the STM32.
- Connect the output of the voltage divider to an ADC pin.
**Supercapacitor:**

- For supercapacitors, the energy is directly proportional to the square of the voltage:
 E = 1/2 C V^2
    - Measuring voltage allows you to calculate energy, provided you know the capacitance.

https://www.youtube.com/watch?v=EsZLgqhqfO0
ADC1_IN8 PA3 A2 - reading pin 
![[Pasted image 20241203172736.png]]
We have been succesfull in reading voltage on a potenciometer
Measuring capacitor: 
**Hardware Setup**:

- Connect one terminal of the capacitor to the ADC pin (PA2).
- The other terminal should be connected to ground (GND).
- Ensure the voltage across the capacitor does not exceed the reference voltage of the ADC (typically 3.3V). If needed, use a voltage divider.
![[Pasted image 20241204111530.png]]
since we are programing the board (using UBS power source +5V) but we also want to match the ADCs Voltage raw data rating with the Voltage measured... Following will be used and might be changed later when we power the board with the 3.3V power source.

Voltage devider power source:

V_ref = ( 3.3 / +5 ) V 

V_cap = (ADC_raw * V_ref) / 4095

R2 / (R2 + R1) * V_ref = 3.3V
-> 2.5 R2 = 3.3 R1

I chose 39Kohm and 30Kohm ... 
but now i noticed that the ratio is correct to the source ... meaning 6V is about 4025 raw ADC data value

and in devision 39Kohm -> 1883
 WE WANR A HUGE RESISTOR ! FOR LESS CONSUMPTION 
 AND WE WANT TO READ ONLY ONE WHEN NEEDED



New plan : Use status 1,2,3 pins to see if we are running out of power
![[Pasted image 20241204133049.png]] 

### Decision making
## UART Communication with LoRa-Module
For the LoRa-Module, we utilize the Wio-e5 mini board.
![[lora_e5_mini_pinout.jpg]]
*[Wio-e5 mini pinout](https://wiki.seeedstudio.com/LoRa_E5_mini/)*

For our communication to work we need to perform three tasks:
- Install a serial console for [UART communications](SSR_Portfolio/Pages/LoRa/UART%20Communications).
- Set up [TheThingsNetwork](SSR_Portfolio/Pages/LoRa/TheThingsNetwork).
- Create a [back-end](SSR_Portfolio/Pages/LoRa/Back-End) that collects the data and visualizes it.

If we follow these pages we should get a connection as follows:
![[lora_working.png]]

*Some handy videos include: [setup device](https://www.youtube.com/watch?v=L_acKpwNvnc&list=WL&index=11&t=600s) & [setup MQTT](https://www.youtube.com/watch?v=9H6GFXatOCY&list=WL&index=12&t=128s)*

![[Wio-E5 Datasheet.pdf]]

==Tom==

## I2C Communication with Rover Bot
==Tom==

