# Table of contents
==To Be Generated At the end==

# SSR
By Adam Hejduk, Robbe Elsermans, and Thomas Kramp
## Intro
As humanities interest in Mars grows, an understanding of its weather is needed. One of the greatest challenges that the planet provides are the frequent dust storms.
This project will try to monitor the weather phenomenon (such as temperature, humidity, light, ...) to predict such storms.
## Board selection
[[Brain Board selection]]
## MVP
The MVP (Minimal Vital Product) contains the following items:
- LoRa module connection that transmits data.
- Dashboard to review received data.
- The ability to measure temperature and humidity.
- Only measure when there is enough energy available.
- ..?

## Power Profiling
[[Power Profiling]]
## Brain code
[[STM32L412KB]]
## BLE Module
[[nRF52 SEEED XIAO]]
## I2C communication sensor SHT40
[[SHT40]]
## I2C communication of LTR-329



![[Lite-On_LTR-329ALS-01 DS_ver1.1-348647.pdf]]
==For Adam==


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

