# SSR
By Thomas Kramp, Robbe Elsermans, and Adam Hejduk

## Intro

## Board selection
For our project, we had a STM32L4 available which needs to be the brain. Furthermore, a seeed XIAO nRF52840 board is provided to be used as BLE module. Although, the seeed XIAO nRF52840 has a MCU with 10 available IO's. Thus, if we count our needed IO's and pheriperals there is a change to use the seeed instead of an extra STM32.
![[block diagram.png]]
As can be observed from the block diagram there is need for five IO's, one I2C interface, one 1-Wire interface
