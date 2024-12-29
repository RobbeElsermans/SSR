# Account creation
Go to [TheThingsNetwork website](https://www.thethingsnetwork.org/) and click the Sign Up button:

![ttn sign up](../../Images/LoRa/ttn_sign_up_1.png)

Scroll down and click the *"Join the things network"* button:

![ttn join up](../../Images/LoRa/ttn_join.png)

Click the *"Sign in for free"*:

![ttn sign up](../../Images/LoRa/ttn_sign_up_2.png)

Create and account:

![ttn create account](../../Images/LoRa/ttn_create_account.png)

After creating and verifying your account, log in:

![ttn login](../../Images/LoRa/ttn_login.png)

# Open the console
Click on the console button:

![ttn console](../../Images/LoRa/ttn_console.png)

Select the closest cluster available *(Ireland)*:

![ttn cluster](../../Images/LoRa/ttn_cluster.png)

You should see the following interface:

![ttn console interface](../../Images/LoRa/ttn_console_interface.png)

# Connect device
## Create an application
Hover over the plus sign and click on ""Add application":

![ttn create application](../../Images/LoRa/ttn_create_application.png)

Use the following settings to create an application:

![ttn application settings](../../Images/LoRa/ttn_application_settings.png)

## Add a device
Under the "End devices" tab, click op the "Register end device" button:

![ttn register device](../../Images/LoRa/ttn_register_device.png)

Use the following settings to register an end device:
- DevEUI should be provided by the device. Use the device value.
- Device address, AppSKey & NwkSKey should be generated and then configured onto the end device.

![ttn end device type](../../Images/LoRa/ttn_end_device_type.png)

![ttn advanced activation settings](../../Images/LoRa/ttn_advanced_activation_settings.png)

![ttn provisioning information](../../Images/LoRa/ttn_provisioning_information.png)

### Create a MQTT network connection
Click on the left, onto the "Intigrations" tab and afterwards on the "MQTT" sub-tab.
The following settings can be used to subscribe onto MQTT messages of this end device.
*Note that the password needs to be generated.*

![ttn mqtt network settings](../../Images/LoRa/ttn_mqtt_network_settings.png)