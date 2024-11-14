UART communication is needed for the LoRa module, since it uses AT commands. To connect via UART, we can use any serial monitor. Examples are:
- [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
- [Arduino serial monitor](https://www.arduino.cc/en/software)
*We will proceed with Putty.*
# Putty for Linux *(Debian)*
Use the following commands to install Putty on a Debian distribution.
- Update system
``` bash
sudo apt update && sudo apt upgrade
```
- Install Putty
``` bash
sudo apt install putty putty-tools
```
- Verify installation
``` bash
putty --version
```
# UART settings
Open the serial monitor application and use the following settings:
- Baud-Rate:     9600
- Data Bits:        8
- Stop Bit:          1
- Parity:             None
- Flow Control: None
![UART Settings](/SSR_Portfolio/Images/LoRa/putty_uart_settings.png)
# Test connection
To test the connection, we just need to use the base AT command.
``` Serial
AT
```
![connection_test_output]()
# Helpful commands
Enter the test mode
``` Serial
AT+mode="LWABP"
```

Select a channel
``` Serial
AT+ch=num,0
```

Show device ids
``` Serial
AT+id
```

Set device address
``` Serial
AT+id=devaddres,"*address*"
```

Set device secret keys
``` Serial
AT+id=appskey,"*AppSKey*"
AT+id=nwkskey,"*NwkSKey*"
```

Set device identifier
``` Serial
AT+id=deveui,"*DevEUI*"
AT+id=appeui,"*DevEUI*"
```

Set device data rate
``` Serial
AT+ch=dr,7
```

Sent a message
``` Serial
AT+msg="*message*"
```
