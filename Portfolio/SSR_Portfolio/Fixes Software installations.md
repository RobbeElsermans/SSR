CubeMX 
-> https://community.st.com/t5/stm32cubemx-mcus/stm32cubemx-6-5-0-quot-please-install-java-jre-11-0-10-or-a-more/td-p/83880
```
sudo chmod 755 jre/bin/*
```
VSCode upload SMT32 code
```bash
sudo wget https://raw.githubusercontent.com/raspberrypi/openocd/rp2040/contrib/60-openocd.rules
sudo usermod -a -G plugdev {user}
reboot
```
```bash
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
```
```bash
sudo systemctl restart udev
```

MX-Cube-EEPROM failed to load
https://community.st.com/t5/stm32cubemx-mcus/how-to-install-x-cube-eeprom-on-stm32cube/td-p/78773
https://community.st.com/t5/stm32cubemx-mcus/how-can-i-install-and-use-x-cube-eeprom-middleware-in/m-p/122931
Follow this link https://community.st.com/t5/stm32-mcus/porting-and-using-x-cube-eeprom-with-any-stm32/ta-p/570539