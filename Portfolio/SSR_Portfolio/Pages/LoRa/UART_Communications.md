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
![[uart_settings.png]]
# Test connection
To test the connection, we just need to use the base AT command.
``` Serial
AT
```
![[uart_test_output.png]]
# Setup connection to TheThingsNetwork
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
// With 260BC2D8 the address
AT+id=devaddr,"260BC2D8"
```

Set device secret keys
``` Serial
// With E91661F4C1D5172FE116CCDB137A3FDE the AppSKey
// With 06E55AEC9813D87693F0A42EAF441E93 the NwkSKey
AT+key=appskey,"E91661F4C1D5172FE116CCDB137A3FDE"
AT+key=nwkskey,"06E55AEC9813D87693F0A42EAF441E93"
```

Set device identifier
``` Serial
// With 2CF7F12052608782 as the DevEUI
AT+id=deveui,"2CF7F12052608782"
AT+id=appeui,"2CF7F12052608782"
```

Enter the test mode
``` Serial
AT+mode="LWABP"
```

Set device data rate
``` Serial
AT+dr=7
```

Set channel
``` Serial
AT+ch=0
```

![[uart_settings_check.png]]

Sent a message
``` Serial
AT+msg="*message*"
```
