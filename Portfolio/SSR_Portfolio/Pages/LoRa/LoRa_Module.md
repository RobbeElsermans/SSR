For the LoRa-Module, we utilize the [Wio-e5 mini board](https://wiki.seeedstudio.com/LoRa_E5_mini/).

![lora_e5_mini_pinout](../../Images/LoRa/module/lora_e5_mini_pinout.jpg)
*[Wio-e5 mini pinout](https://wiki.seeedstudio.com/LoRa_E5_mini/)*
# Communications
## Diagram
The base communication flow is depicted in the block schematic below.

![lora_communications](../../Images/LoRa/lora_communications.png)
## Setup
To setup this network we will need to follow these steps:
1) Connect to the LoRa module via UART/Serial to change the modules configuration.
2) Connect the module to TheThingsNetwork *(TTN)*
3) Create a Node-Red environment and connect it to TTN network.
# Setup
## Connect to LoRA
First we will connect to the LoRa module, to change the modules configuration. This can be done via the USB-C port and a serial monitor. *This can be done via a program such as Putty or the ArduinoIDE serial terminal. ([Installation and set](Setup_LoRaWAN_progams.md))*
### UART/Serial settings
The standard UART/Serial settings the LoRa module uses are as follows:
- Baud-Rate:     9600
- Data Bits:        8
- Stop Bit:          1
- Parity:             None
- Flow Control: None

![uart_settings](../../Images/LoRa/module/uart_settings.png)
### AT commands
The LoRa module makes use of AT commands. All these commands can be found in this [document](../../Datasheets/Lora_Specifications.pdf).

To test the connection, we need to send the base AT command:
``` AT Command
AT
```

This should return an "OK"-message.

![at_command_test_output](../../Images/LoRa/module/at_command_test_output.png)

Once the connection has been set up and checked, we can start with getting the current configuration. This can be done using the following command:
``` AT Command
AT+ID
```

![at_command_config_output](../../Images/LoRa/module/at_command_config_output.png)

These can later be used to add the device to TTN.
### Configure mode
Since the communication from the rover to the servers is only one way, we decided to use ABP. In this manner the configuration can be hard-coded on the device. This can be done using the following command:
``` AT Command
AT+MODE="LWABP"
```

![at_command_set_mode_output](../../Images/LoRa/module/at_command_set_mode_output.png)

The device address can be changed using the following command:
``` AT Command
// With 260BC2D8 the address
AT+ID=DevAddr,"260BC2D8"
```

![at_command_set_devaddr_output](../../Images/LoRa/module/at_command_set_devaddr_output.png)

The device secret keys can be changed using the following commands:
``` AT Command
// With E91661F4C1D5172FE116CCDB137A3FDE the AppSKey
// With 06E55AEC9813D87693F0A42EAF441E93 the NwkSKey
AT+KEY=AppSKey,"E91661F4C1D5172FE116CCDB137A3FDE"
AT+KEY=NwkSKey,"06E55AEC9813D87693F0A42EAF441E93"
```

![at_command_set_appskey_output](../../Images/LoRa/module/at_command_set_appskey_output.png)
![at_command_set_nwkskey_output](../../Images/LoRa/module/at_command_set_nwkskey_output.png)

The device identifiers can be changed using the following commands:
``` AT Command
// With 2CF7F12052608782 as the DevEUI
AT+ID=DevEui,"2CF7F12052608782"
AT+ID=AppEui,"2CF7F12052608782"
```

![at_command_set_deveui_output](../../Images/LoRa/module/at_command_set_deveui_output.png)
![at_command_set_appeui_output](../../Images/LoRa/module/at_command_set_appeui_output.png)

### Configure channel communication
As previously established the module uses ABP mode.
``` AT Command
AT+MODE="LWABP"
```

![at_command_set_mode_output](../../Images/LoRa/module/at_command_set_mode_output.png)

To minimize the energy consumption *(and maximize data-rate for testing purposes)*, the LoRa module is set to "DR7". This has the lowest range with highest data rate. 
``` AT Command
AT+DR=7
```

![at_command_set_data_rate_output](../../Images/LoRa/module/at_command_set_data_rate_output.png)

For implementation on Mars, a higher range is better used *(DR0 with SF12 has a 5 times range increase)* and tested for energy consumption. This was not achieved in this project due to time constraints. 

A channel can be selected using the following command:
``` AT Command
AT+CH=num,0
```

![at_command_set_channel_output](../../Images/LoRa/module/at_command_set_channel_output.png)

The settings of this specific channel can then be changed using the following command:
``` AT Command
AT+CH=0, 868.1, DR0, DR5
```

![at_command_channel_settings_output](../../Images/LoRa/module/at_command_channel_settings_output.png)

The settings of this channel can be checked using the following command:
``` AT Command
AT+CH=0
```

![at_command_channel_settings_output](../../Images/LoRa/module/at_command_channel_settings_output.png)
### Send a message
Send a string message:
``` AT Command
AT+MSG="*message*"
```

![at_command_send_message_output](../../Images/LoRa/module/at_command_send_message_output.png)

Send a hexadecimal message:
``` AT Command
AT+MSGHEX="*message*"
```

### ==Other interesting commands==
``` AT Command
AT+LOWPOWER
```

``` AT Command
AT+POWER
```

``` AT Command
AT+EEPROM
```

### ==Error messages==

![at_command_error_output](../../Images/LoRa/module/at_command_error_output.png)
## Add the device to TTN

## Create a dashboard

# Implementation

``` AT Command
AT+MODE="LWABP"
AT+ID=DevAddr,"260BC2D8"
AT+KEY=AppSKey,"E91661F4C1D5172FE116CCDB137A3FDE"
AT+KEY=NwkSKey,"06E55AEC9813D87693F0A42EAF441E93"
AT+ID=DevEui,"2CF7F12052608782"
AT+ID=AppEui,"2CF7F12052608782"

AT+MODE="LWABP"
AT+DR=7
AT+CH=num,0
AT+MSGHEX="*message*"
```