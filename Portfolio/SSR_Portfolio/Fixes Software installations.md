CubeMX 
-> https://community.st.com/t5/stm32cubemx-mcus/stm32cubemx-6-5-0-quot-please-install-java-jre-11-0-10-or-a-more/td-p/83880
```
sudo chmod 755 jre/bin/*
```
VSCode upload SMT32 code
```bash
sudo usermod -a -g plugdev
```
```bash
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
```
```bash
sudo systemctl restart udev
```
