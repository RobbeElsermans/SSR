#include "DriverTWISlave.h"

/* Private variables */
static volatile uint8_t *Twis_rxBuffer;      // Receive buffer
static volatile uint8_t Twis_rxIndex;        // Receive buffer index
static volatile uint8_t Twis_rxLength;       // Receive buffer length
static volatile uint8_t Twis_status;         // Status of the slave
static volatile uint8_t Twis_error;          // Error status

static SemaphoreHandle_t SemaRxComplete = NULL;

/* Public function to initialize the slave */
void DriverTWISInit(TWI_t *twi, uint8_t *rxBuffer, uint8_t rxLength) {
	Twis_rxBuffer = rxBuffer;
	Twis_rxLength = rxLength;

	Twis_rxIndex = 0;
	Twis_status = 0;
	Twis_error = 0;

	/* Initialize the LED driver */
	DriverLedInit();

	/* Set the slave address to 0x37 */
	twi->SLAVE.ADDR = 0x37 << 1; // 7-bit address

	/* Configure SDA hold time */
	twi->SLAVE.CTRLA = TWI_SLAVE_ENABLE_bm | // Enable TWI Slave
	TWI_SLAVE_PIEN_bm |   // Enable Stop Interrupt
	TWI_SLAVE_DIEN_bm |   // Enable Data Interrupt
	TWI_SLAVE_APIEN_bm;   // Enable Address Interrupt

	/* Set the address mask to respond to only specified address */
	twi->SLAVE.ADDRMASK = 0x00; // Mask to match the exact address

	/* Create semaphore for signaling */
	SemaRxComplete = xSemaphoreCreateBinary();
}

/* ISR to handle TWI Slave events */
ISR(TWI_vect) {
	TWI_t *twi = &TWIC; // Replace TWIC with the correct TWI instance
	uint8_t status = twi->SLAVE.STATUS;

	/* Address Match Interrupt */
	if (status & TWI_SLAVE_APIF_bm) {
		Twis_rxIndex = 0;

		/* Clear interrupt flag */
		twi->SLAVE.STATUS = TWI_SLAVE_APIF_bm;
	}

	/* Data Interrupt */
	if (status & TWI_SLAVE_DIF_bm) {
		/* Master writing to slave */
		if (!(status & TWI_SLAVE_DIR_bm)) {
			if (Twis_rxIndex < Twis_rxLength) {
				uint8_t receivedData = twi->SLAVE.DATA;
				Twis_rxBuffer[Twis_rxIndex++] = receivedData;

				/* Check for the specific command to turn on the LED */
				if (receivedData == 0x10) {
					DriverLedSet(0x01); // Set LED 0 (PB0) ON
				}
				} else {
				Twis_error = 1; // Buffer overflow
			}
		}

		/* Clear data interrupt flag */
		twi->SLAVE.STATUS = TWI_SLAVE_DIF_bm;
	}

	/* Stop Interrupt */
	if (status & TWI_SLAVE_APIF_bm) {
		/* Clear stop interrupt flag */
		twi->SLAVE.STATUS = TWI_SLAVE_APIF_bm;

		/* Signal Rx completion if data was received */
		if (Twis_rxIndex > 0) {
			xSemaphoreGiveFromISR(SemaRxComplete, NULL);
		}
	}
}

/* Function to wait for reception completion */
uint8_t TWISWaitForRxComplete(TickType_t timeout) {
	return xSemaphoreTake(SemaRxComplete, timeout) == pdTRUE ? 1 : 0;
}
