# SSR
By Thomas Kramp, Robbe Elsermans, and Adam Hejduk

## Intro

## Board selection
For our project, we had a STM32L4 available which needs to be the brain. Furthermore, a seeed XIAO nRF52840 board is provided to be used as BLE module. Although, the seeed XIAO nRF52840 has a MCU with 10 available IO's. Thus, if we count our needed IO's and pheriperals there is a change to use the seeed instead of an extra STM32.
![[block diagram.png]]
As can be observed from the block diagram there is need for:
- four IO's, 
- one ADC, 
- one I2C interface, 
- and one 1-Wire interface.
In total, these are 10-pins.

In order to check which MCU we are going to use some comparison is needed.
### STM32L4 VS nRF52840

| MCU      | Power (V) | Flash (kB) | SRAM (40kB) | Sleep (µA) at 3V | Deep Sleep (µA) at 3V | IO  | Clock (MHz) |
| -------- | --------- | ---------- | ----------- | ---------------- | --------------------- | --- | ----------- |
| STM32L4  | 1.7-3.6   | 128        | 40          |                  |                       | 20  | 24          |
| nRF52840 | 1.7-5.5   | 1024       | 256         | 3.16             | 0.4                   | 11  | 64          |
