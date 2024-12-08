#include "StartupTask.h"

#include "FreeRTOS.h"
#include "task.h"

#include "DriverUSART.h"
#include "DriverCursorStick.h"
#include "DriverPower.h"
#include "DriverTwiMaster.h"
#include "Driverpl9823.h"
#include "DriverAdc.h"
#include "DriverLed.h"
#include "DriverMPU6050.h"
#include "DriverDbgUSART.h"
#include "DriverAdps9960.h"
#include "DriverOled.h"
#include "DriverVL53L0X.h"
#include "DriverMotor.h"

#include "OledMenuTask.h"
#include "MotorPosTask.h"
#include "MotorSpeedTask.h"
#include "ADCTask.h"
#include "LineFollowerSpeedTask.h"
#include "LineFollowerDirectTask.h"
#include "RGBTask.h"
#include "GyroTask.h"
#include "TerminalTask.h"
#include "MotionTask.h"

#include <stdio.h>


//Private function prototypes
static void WorkerStartup(void *pvParameters);

//Function definitions
void InitStartupTask()
{
	xTaskCreate( WorkerStartup, "startup", 256, NULL, tskIDLE_PRIORITY+3, NULL );	
}

static void WorkerStartup(void *pvParameters)
{
	int res;
	
	DriverPowerVccAuxSet(1);	//Enable Auxillary power line
	DriverCursorstickInit();	//Initialize cursor stick
	//DriverLedInit();			//Initialize LED's
	//DriverUSARTInit();		//USART init and link to stdio
	DriverPowerInit();		//Initialize aux power driver
	
	//DriverTWIMInit();			//Initialize TWI in master mode <-- oled
	DriverPL9823Init();		//Initialize PL9823 LEDs <--- Disable RGB leds
	//DriverAdcInit();			//Initialize ADC driver
	
	//DriverOLEDInit(2);		//Initialize OLED display
	//DriverAdps9960Init();		//Initialize color sensor	
	//DriverVL53L0XInit();		//Initialize rangefinder

	DriverMotorInit();
	
	vTaskDelay(50);
	
	//Enable test output (T21)
	//PORTA.DIRSET=1<<5;	
	//Initialize application tasks			
	
	//InitOLEDMenuTask(); // <-- oled
	//InitADCTask();
	InitMotorPosTask();
	InitMotorSpeedTask();
	//InitLineFollowerSpeedTask();
	
	//InitLineFollowerDirectTask();
	InitRGBTask(); // <--- Disable RGB leds
	//InitGyroTask();
	//InitTerminalTask();
	//InitMotionTask();
	
	MotorSpeed();
	
	vTaskSuspend(NULL);

}

void MotorSpeed() {
	uint8_t Stick;
	int16_t Speed1=0,Speed2=0;
	char s[32];
	
	while (1) {
		Stick=DriverCursorStickGetFifo(300);
		
		if (Stick & CURSOR_UP) {
			EnableMotorPosTask();
			EnableMotorSpeedTask();
			Speed1 = 1000;
			Speed2 = 1000;
		}
		if (Stick & CURSOR_DOWN) {
			EnableMotorPosTask();
			EnableMotorSpeedTask();
			Speed1 = -1000;
			Speed2 = -1000;
		}
		if (Stick & CURSOR_RIGHT) {
			EnableMotorPosTask();
			EnableMotorSpeedTask();
			Speed1 = -1000;
			Speed2 = 1000;
		}
		if (Stick & CURSOR_LEFT) {
			EnableMotorPosTask();
			EnableMotorSpeedTask();
			Speed1 = 1000;
			Speed2 = -1000;
		}
		if (Stick & CURSOR_PRESS) {
			MotorSpeedSet(0,0);
			vTaskDelay(300);
			DisableMotorSpeedTask();
			DisableMotorPosTask();
		}
		MotorSpeedSet(Speed1,Speed2);
	}
}
