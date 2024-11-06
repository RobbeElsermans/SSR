
https://wiki.seeedstudio.com/xiao_nrf52840_with_platform_io/
https://wiki.seeedstudio.com/XIAO_BLE/
https://docs.platformio.org/en/stable/boards/ststm32/nucleo_l412kb.html#board-ststm32-nucleo-l412kb
https://www.st.com/en/development-tools/stm32cubemx.html

https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/bluefruit.h


A link to use the flash as sort of "EEPROM" to not lose any data.
https://www.seeedstudio.com/blog/2021/04/26/managing-arduino-memory-flash-sram-eeprom/
PROGMEM is used, but is only read-only.

A link to emulate flash as eeprom on STM32 boards
https://community.st.com/t5/stm32-mcus-products/how-to-use-flash-memory-as-a-eeprom-in-stm32g071cbt6/td-p/585659


A link where STM32L4 examples are located
https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/NUCLEO-L412KB/Examples/PWR
These are some low power examples. Good documentation!
*Table 43. Current consumption in Stop 2 mode* and *I DD_ALL (Stop 2 with RTC)* is a good table to go through. A internal LSI is used. A current of 1.3-0.98 µA is used here.
> Bit 11 ENULP: Enable ULP sampling
When this bit is set, the BORL, BORH and PVD are periodically sampled instead continuous
monitoring to reduce power consumption. Fast supply drop between two sample/compare
phases will not be detected in this mode. This bit has impact only on STOP2, Standby and
shutdown low power modes.(1)
>Bit 12 LPCAL: Calibration low-power mode
0: Calibration window is 220 RTCCLK, which is a high-consumption mode. This mode should
be set only when less than 32s calibration window is required.
1: Calibration window is 220 ck_apre, which is the required configuration for ultra-low
consumption mode.

A I2C scanner in STM32
https://deepbluembedded.com/stm32-i2c-scanner-hal-code-example/