// https://ww1.microchip.com/downloads/en/DeviceDoc/doc8077.pdf#page=219

#ifndef DRIVERTWISLAVE_H
#define DRIVERTWISLAVE_H

#include <stdint.h>
#include <avr/io.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "DriverLed.h"

/* Function Prototypes */
void DriverTWISInit(TWI_t *twi, uint8_t *rxBuffer, uint8_t rxLength);
uint8_t TWISWaitForRxComplete(TickType_t timeout);

#endif // DRIVERTWISLAVE_H
