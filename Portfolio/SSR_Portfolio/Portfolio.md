# Table of contents
==To Be Generated At the end==

# SSR
By Adam Hejduk, Robbe Elsermans, and Thomas Kramp
## Intro
As humanities interest in Mars grows, an understanding of its weather is needed. One of the greatest challenges that the planet provides are the frequent dust storms.
This project will try to monitor the weather phenomenon (such as temperature, humidity, light, ...) to predict such storms.
## Project goal
Each system operates autonomously as its own master, navigating the surface of Mars independently. When a node becomes lost or encounters an issue, and another node approaches, they can exchange valuable information to assist each other. Otherwise, each node functions independently.

BLE technology is used to detect proximity between nodes and share crucial information about their headings, preventing collisions. Additionally, if a node encounters a mechanical problem and becomes immobile, it can still transmit its data to nearby nodes and gateways for continued operation and coordination.
## Project division
#### Adam
- Environmental Sensing​
- Energy harvesting ​
- Energy output management​
- Soldering man

#### Thomas
- LoRa connectivity
- Dashboard
- Gyroscope
- Rover actuation
#### Robbe
- Project management
- BLE intercommunication
- Power Profiling
- Energy Awareness
## Board selection
[Brain Board Selection page](Pages/Investigation/Brain_Board_Selection.md)

## MVP
The MVP (Minimal Vital Product) contains the following items:
- LoRa-module that transmits data to a gateway.
- Dashboard to review received data from LoRa-module.
- The ability to measure temperature and humidity.
- Energy awareness
- Use environmental source as energy source
![Block Diagram](Images/block_diagram.pdf)
## Power Profiling
[Power Profiling page](Pages/Power_Profiling/Power_Profiling_V1.md)
## STM32 Module
[STM32 module page](Pages/Brain_module/STM32L412KB.md)
## BLE Module
[BLE Module page](Pages/BLE_Module/nRF52_SEEED_XIAO.md)
## SHT40 (temperature and humidity)
[SHT40 page](Pages/Sensor/SHT40.md)
- addressing an array x addressing a place in an array 
- UART printing is not the same as printf sprintf...
- HAL libary learining
- working with datasheets of more complex nature for the first time 
- programing in more dvanced C


## LTR-329 (light)
[LTR-329 page](Pages/Sensor/LTR-329.md)

## Energy Harvesting
[Energy Harvesting page](Pages/Energy_Harvesting/Energy_Harvesting.md)

## UART Communication with LoRa-Module
For the LoRa-Module, we utilize the Wio-e5 mini board.
![lora_e5_mini_pinout](Images/LoRa/lora_e5_mini_pinout.jpg)
*[Wio-e5 mini pinout](https://wiki.seeedstudio.com/LoRa_E5_mini/)*

For our communication to work we need to perform three tasks:
- Install a serial console for [UART communications](SSR_Portfolio/Pages/LoRa/UART%20Communications).
- Set up [TheThingsNetwork](SSR_Portfolio/Pages/LoRa/TheThingsNetwork).
- Create a [back-end](SSR_Portfolio/Pages/LoRa/Back-End) that collects the data and visualizes it.

If we follow these pages we should get a connection as follows:
![lora_working](Images/LoRa/lora_working.png)

*Some handy videos include: [setup device](https://www.youtube.com/watch?v=L_acKpwNvnc&list=WL&index=11&t=600s) & [setup MQTT](https://www.youtube.com/watch?v=9H6GFXatOCY&list=WL&index=12&t=128s)*

[Wio-E5 Datasheet](Datasheets/Wio-E5_Datasheet.pdf)

==Tom==

## I2C Communication with Rover Bot
==Tom==

