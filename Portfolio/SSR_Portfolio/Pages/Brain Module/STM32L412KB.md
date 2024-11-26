## Code Flow
Our brain, STM32L412KB, will have a certain flow as depicted in the block schematic below.

![brain_flow_diagram](../../Images/brain_flow_diagram.pdf)

This is coded in a finite state machine which cycles through the different states. This to provide a minimal code footprint onto the MCU.