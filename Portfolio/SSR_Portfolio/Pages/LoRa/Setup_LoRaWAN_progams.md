# Putty installation for Linux *(Debian)*
To install [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) on a Debian distribution use the following commands:
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

## Putty crash
If the terminal crashes after opening the connection, it might be due to the default font being faulty. Use the following settings:

![uart font settings](uart_font_settings.png)

# Arduino Installation
To install the ArduinoIDE, download the right package form this [link](https://www.arduino.cc/en/software).