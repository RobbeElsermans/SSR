# SSR
By Adam Hejduk, Robbe Elsermans, and Thomas Kramp

## Intro
As humanities interest in Mars grows, an understanding of its weather is needed. One of the greatest challenges that the planet provides are the frequent dust storms.
This project will try to monitor the weather phenomenon (such as temperature, humidity, light, ...) so we can possibly predict such storms.

## Board selection
For our project, we had a STM32L4 available which needs to be the brain. Furthermore, a seeed XIAO nRF52840 board is provided to be used as BLE module. Although, the seeed XIAO nRF52840 has a MCU with 10 available IO's. Thus, if we count our required IO's and peripherals there is a change to use the seeed instead of an extra STM32 to act as the brain.
![[block diagram.png]]
As can be observed from the block diagram there is need for:
- four IO's, 
- one ADC, 
- one I2C interface, 
- and one 1-Wire interface.
In total, these are 10-pins. This case is when we use the L412 as the brain and SEEED XIAO as a BLE module.

When nRF52840 is the brain, 10 pins are needed. 

In order to select which MCU we are going to use some comparison is required.
### STM32L4 VS nRF52840

| MCU      | Power (V) | Flash (kB) | SRAM (40kB) | Sleep (µA) at 3V | Deep Sleep (µA) at 3V | IO  | Clock (MHz) |
| -------- | --------- | ---------- | ----------- | ---------------- | --------------------- | --- | ----------- |
| STM32L4  | 1.7-3.6   | 128        | 40          |                  |                       | 20  | 24          |
| nRF52840 | 1.7-5.5   | 1024       | 256         | 3.16             | 0.4                   | 11  | 64          |
