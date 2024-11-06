# Table of contents
- [[#Intro|Intro]]
- [[#Board selection|Board selection]]
	- [[#Board selection#STM32L4 VS nRF52840|STM32L4 VS nRF52840]]
- [[#MVP|MVP]]
- [[#Brain code|Brain code]]
- [[#Joule Scope Measurements|Joule Scope Measurements]]
	- [[#Joule Scope Measurements#STM32L4 standby|STM32L4 standby]]
- [[#I2C communication sensor SHT40|I2C communication sensor SHT40]]

# SSR
By Adam Hejduk, Robbe Elsermans, and Thomas Kramp
## Intro
As humanities interest in Mars grows, an understanding of its weather is needed. One of the greatest challenges that the planet provides are the frequent dust storms.
This project will try to monitor the weather phenomenon (such as temperature, humidity, light, ...) to predict such storms.

## Board selection
For our project, we had a STM32L4 available which needs to be the brain. Furthermore, a seeed XIAO nRF52840 board is provided to be used as BLE module. Although, the seeed XIAO nRF52840 has a MCU with 10 available IO's. Thus, if we count our required IO's and peripherals there is a change to use the seeed instead of an extra STM32 to act as the brain.
![[block diagram.pdf]]
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

## MVP
The MVP (Minimal Vital Product) contains the following items:
- LoRa module connection that transmits data.
- Dashboard to review received data.
- The ability to measure temperature and humidity.
- Only measure when there is enough energy available.
- ..?

## Brain code
Our brain, STM32L412KB, will have a certain flow as depicted in the block schematic below.
![[brain_flow_diagram.pdf]]
This is coded in a finite state machine which cycles through the different states. This to provide a minimal code footprint onto the MCU.

## Joule Scope Measurements
### STM32L4 standby
This measurement is done to identify and verify that the datasheet is correct about the $0.775-0.565 µA$ in Deep Sleep. 

The test code will have a counter that is increased every startup. This counter is saved in the virtual EEPROM. This counter states how many times a led must blink at startup,Thus, it is easly verified at which cycle the STM32L4 is in.

The Standby code will last for $33$ seconds when we use a prescaler of $/16$ at the Low power Oscillator of $\pm 32kHz$ and a compare value of $0xFFFF$ which yields in $\frac{16}{\pm32kHz} = 0.5ms$ and
$0.5ms \times 2^16 = 32.768 \Rightarrow \pm 33 seconds$.

==ADD figure of JouleScope==

## I2C communication sensor SHT40
![[HT_DS_Datasheet_SHT4x.pdf]]
Some conclusions: 
- address can range from 0x44 to 0x46.
- Idle current = $0.08\mu A$
- Power up current = $50\mu A$
- Measurement $320\mu A$
- 3 repeatability modes to receive the mean values. Each their own current consumption.
- Read (high prec.) 0xFD -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
- Read (med prec.) 0xF6 -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
- Read (low prec.) 0xE0 -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
$RH = ( -6 + 125 \cdot \frac{S_{RH}}{2^{16} -1}) (\%RH)$
$T = ( -45 + 175 \cdot \frac{S_{T}}{2^{16} -1}) (^\circ C)$



