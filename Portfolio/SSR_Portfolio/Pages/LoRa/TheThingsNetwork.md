# Account creation
Go to [TheThingsNetwork website](https://www.thethingsnetwork.org/) and click the Sign Up button:
![[TheThingsNetwork sign up](ttn_sign_up_1.png)

Scroll down and click the *"Join the things network"* button:
![[TheThingsNetwork join](ttn_join.png)

Click the *"Sign in for free"*:
![[TheThingsNetwork sign up](ttn_sign_up_2.png)

Create and account:
![[TheThingsNetwork create account](ttn_create_account.png)

After creating and verifying your account, log in:
![[TheThingsNetwork login](ttn_login.png)
# Open the console
Click on the console button:
![[TheThingsNetwork console](ttn_console.png)

Select the closest cluster available *(Ireland)*:
![[TheThingsNetwork cluster](ttn_cluster.png)

You should see the following interface:
![[TheThingsNetwork console interface](ttn_console_interface.png)
# Connect device
## Create an application
Hover over the plus sign and click on ""Add application":
![TheThingsNetwork create application](ttn_create_application.png)

Use the following settings to create an application:
![TheThingsNetwork application settings](ttn_application_settings.png)
## Add a device
Under the "End devices" tab, click op the "Register end device" knop:
![TheThingsNetwork register device](ttn_register_device.png)

Use the following settings to register an end device:
- JoinEUI should be provided by the device. Use the device value.
- DevEUI and AppKey should be generated and then configured onto the end device.
![TheThingsNetwork create application](ttn_device_settings.png)
### Edit the payload formatter
When in the configuration of a certain end device, click on the "Payload formatters" tab and select the "Custom Javascript formatter" option:
![TheThingsNetwork device formatter](ttn_device_formatter.png)

Then add the following code:
``` js
function decodeUplink(input) {
  return {
    data: {
      bytes: String.fromCharCode.apply(null, input.bytes)
    },
    warnings: [],
    errors: []
  };
}
```

### Create a MQTT network connection
Click on the left, onto the "Intigrations" tab and afterwards on the "MQTT" sub-tab.
The following settings can be used to subscribe onto MQTT messages of this end device.
*Note that the password needs to be generated.*
![TheThingsNetwork MQTT network settings](ttn_mqtt_network_settings.png)