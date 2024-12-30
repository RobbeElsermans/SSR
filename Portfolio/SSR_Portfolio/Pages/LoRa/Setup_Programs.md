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

![uart_font_settings](../../Images/LoRa/module/uart_font_settings.png)

# Arduino Installation
To install the ArduinoIDE, download the right package form this [link](https://www.arduino.cc/en/software).

# TTN account creation
Go to [TheThingsNetwork website](https://www.thethingsnetwork.org/) and click the Sign Up button:

![ttn_sign_up_1](../../Images/LoRa/back_end/ttn_sign_up_1.png)

Scroll down and click the *"Join the things network"* button:

![ttn_join](../../Images/LoRa/back_end/ttn_join.png)

Click the *"Sign in for free"*:

![ttn_sign_up_2](../../Images/LoRa/back_end/ttn_sign_up_2.png)

Create and account:

![ttn_create_account](../../Images/LoRa/back_end/ttn_create_account.png)

After creating and verifying your account, log in:

![ttn_login](../../Images/LoRa/back_end/ttn_login.png)

# Open TTN console
Click on the console button:

![ttn_console](../../Images/LoRa/back_end/ttn_console.png)

Select the closest cluster available *(Ireland)*:

![ttn_cluster](../../Images/LoRa/back_end/ttn_cluster.png)

You should see the following interface:

![ttn_console_interface](../../Images/LoRa/back_end/ttn_console_interface.png)
