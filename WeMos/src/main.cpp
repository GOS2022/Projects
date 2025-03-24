#include <Arduino.h>
#include "gpio.h"
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include "wifiserver.h"
#include "SoftwareSerial.h"
#include "drv_uart.h"

extern "C" {
#include "app_ipl.h"
#include <cont.h>
#include "gos_gcp.h"
#include "gos_task.h"
#include "gos_driver.h"
#include "gos_trace.h"
#include "gos_time.h"
#include "gos.h"
}

#define LED_BUILTINN D4

GOS_STATIC gos_result_t drv_systimer_get (u16_t* pValue);

GOS_STATIC void_t Systick_callback (void_t);

GOS_STATIC Ticker sysTicker;

void setup()
{
  	GOS_STATIC gos_driver_functions_t driverFunctions =
	{
		.shellDriverReceiveChar = drv_uartReceiveBlocking,
		.shellDriverTransmitString = drv_uartTransmitBlocking,
		.traceDriverTransmitString = drv_uartTransmitBlocking,
		.traceDriverTransmitStringUnsafe = drv_uartTransmitBlocking,
		.timerDriverSysTimerGetValue = drv_systimer_get
	};

	drv_uartInit();
  	gos_driverInit(&driverFunctions);  
  	pinMode(LED_BUILTINN, OUTPUT); 
	digitalWrite(LED_BUILTINN, HIGH);
  	//Serial.begin(115200);
  	sysTicker.attach_ms(1, Systick_callback);
  	gos_init();
	app_iplInit();
	//wifiServerInit();
	//svl_ipcInit();
}

void loop()
{
    gos_kernelStart();
}

GOS_STATIC gos_result_t drv_systimer_get (u16_t* pValue)
{
	/*
	 * Local variables.
	 */
	gos_result_t timerDriverGetResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (pValue != NULL)
	{
		*pValue = micros();
		timerDriverGetResult = GOS_SUCCESS;
	}
	return timerDriverGetResult;
}

GOS_STATIC void_t Systick_callback (void_t)
{
  gos_timeIncreaseSystemTime(1);
}