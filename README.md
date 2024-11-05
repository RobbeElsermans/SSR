# SSR
Simple Stupid Rover

A school project where we developed a mars rover that has to be power efficient to last long on harvested energy.

This in combination with low power communication where we use BLE for inter robot communication and LoRa to send out some sensor readings to be monitored on a dashboard.

We use temperature sensor for the environment temperature as wel as an Gyroscope to detect any anomalies when standing still or driving the rover (e.g. bumps, pits).

Lastly, energy awareness is an important key stone of our system as it relies on harvested energy.

## Made by
Adam
Robbe
Thomas

## Projext requirements
### connectivity
- inter rover communication with BLE
- LoRaWAN through a dashboard

### energy
- energy harvesting
- energy awareness
- embedded power optimization

### Sensing
- usefull and feasable

## milestones
- temperature measurement
- gyroscope measurement
- LoRaWAN connection with a NodeRed backend for providing the dashboard
- BLE connection to surrounding SSR's
- Energy measurement (power profile)
- Energy management (optimizing power consumption)

## Group division
### Robbe Elsermans
- BLE connection to surrounding rovers.
- Energy measurement (power profiling)

### Thomas Kramp
- LoRaWAN connectivity to send telemetry data to the servers.
- NodeRed environment to graph all received data.
- Gyroscope implementation.

### Adam hejduk
My Rosposeability is: 
- Sensor of Temperature, Humidity and light
- Energy output management.
15.10.2024 - "I'll research how it works and learn how to code with it." 
