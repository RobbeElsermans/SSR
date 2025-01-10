[Enegry Harvesting Module AEM1094](AEM10941.md)



The goal is to pover as many peripherals as possible... SHT40, LTR-329, STM32, BLE ?
We have:
- Outside S.P.
- Inside S.P.
- 2x 2,7V1F Supercap
- 2x AEM10941 Evaluation Kit 
Note to self: Make sure to have SuperCap connected before connecting power source (S.P.)

Tested Single Cell Configuration: It seems to be ok on 2,7V treshhold, the module gets slighty over 2,7 Volts and than regulates it down so that the Capacitor is okay. 
HighOUT: 1,8V
LowOUT? 1,2V
according to datasheet
It takes about 1h 35m to charge the Cap. to 2,3V WITH additional artiffical lights
![[ChargingOfCap.png]]


### Update on energy harvesting 
Very plesant learning curve
I previously tested only one E.H.M. since i expected them to be the same i only worked with one... Unfortunetly other board has a 'shorted input of source' - it would behave in a way like: if you connect a solar panel the panel would drop its Voltage to about 0.2 V which cant charge a capacitor

#### New way of powering 
We will use a Configuration of Dual-cell supercapacitor (Using the two supercaps in series)
![[DualCellConfig.png]]
2,7 + 2,7 = 5,4 V -> Capacitors should not explode when charged to Max. Threshold
Module should be powering at 3.92V  (HighOut = 3.3V) (LowOut = 1.8V)
If the SuperCaps would reach max threshold Module will regulate the Voltage until 3.6V re reached

After reaching 3.92V on the Dual-cell Capacitor  HighOUT had 3.3V => Success


### Voltage measurement of the energy harvesting module
Hardware Setup: 
- Use the **ADC (Analog-to-Digital Converter)** on the STM32 to measure the voltage of the energy storage device.
- Connect the energy storage's (SUPERCAP) positive terminal to a voltage divider to scale it down if it exceeds the ADC input range of the STM32.
- Connect the output of the voltage divider to an ADC pin.
**Supercapacitor:**

- For supercapacitors, the energy is directly proportional to the square of the voltage:
 E = 1/2 C V^2
    - Measuring voltage allows you to calculate energy, provided you know the capacitance.

https://www.youtube.com/watch?v=EsZLgqhqfO0
ADC1_IN8 PA3 A2 - reading pin 
![[STMConfigVolReading.png]]
We have been succesfull in reading voltage on a potenciometer
Measuring capacitor: 
**Hardware Setup**:

- Connect one terminal of the capacitor to the ADC pin (PA2).
- The other terminal should be connected to ground (GND).
- Ensure the voltage across the capacitor does not exceed the reference voltage of the ADC (typically 3.3V). If needed, use a voltage divider.
  8.2 Mohm resistors 
  5,4/8 200 000 = 6,58537 * 10 ^-7
  
 WE WANT A HUGE RESISTOR ! FOR LESS CONSUMPTION 
 AND WE WANT TO READ ONLY ONE WHEN NEEDED

New plan : Use status 1,2,3 pins to see if we are running out of power - NOT HAPPENING
![[StatusChecksSig.png]] 
### Powering the STM32 Nucleo with the H.M.
![[3V3PowerSuplyNucleo.png]]
Soldering done, together with self current measuring prosedures of the STM32 nucleo B4K... 

We are able to power STM32 with Enviromental sensors and beaconing BLE module !!! 
It can run for about 45 seconds (two beaconing cycles)... then Energy is harvesting for about 7minutes under normal ambient lighting in the lab. before HighVol. Output opens again repeating the cycle.  
![[EnergyHarvestingSTMPoweringBLE.png]]
![[EnergyHarvestingSTMnBLE2.png]]

This energy harvesting module has such a plesant learning curve ! 
I am absolutely using this for some of my own projects in the future. 