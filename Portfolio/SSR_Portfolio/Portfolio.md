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
[[Brain Board selection]]
## MVP
The MVP (Minimal Vital Product) contains the following items:
- LoRa-module that transmits data to a gateway.
- Dashboard to review received data from LoRa-module.
- The ability to measure temperature and humidity.
- Energy awareness
- Use environmental source as energy source
## Power Profiling
[[Power Profiling]]
## STM32 Module
[[STM32L412KB]]
## BLE Module code
[[nRF52 SEEED XIAO]]
## I2C communication sensor SHT40
[[SHT40]]
## I2C communication of LTR-329
[[LTR-329]]
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

