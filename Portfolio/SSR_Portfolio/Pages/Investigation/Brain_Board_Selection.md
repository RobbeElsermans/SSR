For our project, we had a STM32L4 available which needs to be the brain. Furthermore, a seeed XIAO nRF52840 board is provided to be used as BLE module. Although, the seeed XIAO nRF52840 has a MCU with 10 available IO's. Thus, if we count our required IO's and peripherals there is a change to use the seeed instead of an extra STM32 to act as the brain.

![Block Diagram](../../Images/block_diagram.pdf)

As can be observed from the block diagram there is need for:
- four IO's, 
- one ADC, 
- one I2C interface, 
- and one 1-Wire interface.
In total, these are 10-pins. This case is when we use the L412 as the brain and SEEED XIAO as a BLE module.

When nRF52840 is the brain, 10 pins are needed.

In order to select which MCU is the brain, some comparison is required.
### STM32L4 VS nRF52840

| MCU      | Power (V) | Flash (kB) | SRAM (40kB) | Sleep (µA) at 3V | Deep Sleep (µA) at 3V | IO  | Clock (MHz) |
| -------- | --------- | ---------- | ----------- | ---------------- | --------------------- | --- | ----------- |
| STM32L4  | 1.7-3.6   | 128        | 40          | 0.93-1.10        | 0.775-0.565           | 20  | 24          |
| nRF52840 | 1.7-5.5   | 1024       | 256         | 2.35             | 0.4                   | 11  | 64          |
nRF52840 and STM32L4 have different types of sleep modes. The table above is composed with the following assumptions:
- STM32: Sleep mode is supply current in stop mode 2 with RTC enabled & RAM retention (p. 99)
- STM32: Deep Sleep is in Standby mode with RTC enabled & no RAM retention (p. 105).
- nRF52840: Sleep mode RAM is retained and an internal clock/ interrupt is used (p. 56).
- nRF52840: Deep Sleep mode RAM is NOT retained and external interrupt is needed (p. 56).

Conclusion: We continue with STM32L4 which has the ability to use the FLASH as EEPROM where this is not usable in nRF52840 (PROGMEN is read-only so not an option).
With the usage of EEPROM, we can store the data before entering deep sleep and after the wake-up or reset it can fetch the data or add data to it if needed. This is not the case for nRF52840, although, it has better current consumption in Deep Sleep.