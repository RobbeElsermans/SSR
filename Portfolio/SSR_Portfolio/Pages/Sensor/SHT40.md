![[HT_DS_Datasheet_SHT4x.pdf]]

SHT4x is a digital sensor platform for measuring relative humidity and temperature at different accuracy
classes. Its I2C interface provides several preconfigured I2C addresses while maintaining an ultra-low power
budget
![[Pasted image 20250109200430.png]]


![[SHT40 debuging.png]]
Some conclusions: 
- address can range from 0x44 to 0x46.
- Idle current = $0.08\mu A$
- Power up current = $50\mu A$
- Measurement $320\mu A$
- 3 repeatability modes to receive the mean values. Each their own current consumption.
- Read (high prec.) 0xFD -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
- Read (med prec.) 0xF6 -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
- Read (low prec.) 0xE0 -> [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
$RH = ( -6 + 125 \cdot \frac{S_{RH}}{2^{16} -1}) (\%RH)$
$T = ( -45 + 175 \cdot \frac{S_{T}}{2^{16} -1}) (^\circ C)$![[SHT40_standby_mode.png]]


![[sht40_consumption.png]]