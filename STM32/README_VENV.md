Please, in order to convert a generated .ioc file of STM32, use the virtual environment by

```
venv /venv/bin/activate
```

use **stm32pio** to generate the platformio project from an .ioc file generated by CubeMX.

```
pip install stm2pio
python3 -m pip install -U platformio
```

Please note that teh error about GPDSC, doesn't matter
See -> https://github.com/ussserrr/stm32pio/issues/114

and to deactivate
```
deactivate
```

For advanced stuff like MiddleWare applications such as FreeRTOS or any other thing, not done yet.
See below for further investigation
- https://wiki-power.com/en/PlatformIO%E6%90%AD%E9%85%8DCubeMX%E9%A3%9F%E7%94%A8/#platformio-initialization-steps 
- https://community.platformio.org/t/stm32cubemx-and-platformio/38378/3
- https://community.platformio.org/t/minimal-ststm32-freertos-example/37229/3
- https://github.com/platformio/platform-ststm32/tree/master/examples/zephyr-blink/zephyr
- 

sources:
- https://python.land/virtual-environments/virtualenv
- https://github.com/ussserrr/stm32pio
- https://github.com/maxgerhardt/pio-stm32h7-stm32cube-freertos
- https://community.platformio.org/t/platformio-freertos/23554/6