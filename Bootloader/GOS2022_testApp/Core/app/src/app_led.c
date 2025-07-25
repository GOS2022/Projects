/*
 * app_led.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Gabor
 */

#include "app.h"
#include "drv_gpio.h"

#include "iodef.h"

#define LED_TASK_PERIOD     ( 1000u )

GOS_STATIC void_t app_ledTask (void_t);


GOS_STATIC gos_taskDescriptor_t ledTaskDesc =
{
	.taskFunction       = app_ledTask,
	.taskName           = "app_led_task",
	.taskPriority       = 0,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER,
	.taskStackSize      = 0x300
};

gos_result_t app_ledInit (void_t)
{
	gos_result_t ledInitResult = GOS_SUCCESS;

	ledInitResult &= gos_taskRegister(&ledTaskDesc, NULL);

	if (ledInitResult != GOS_SUCCESS)
	{
		ledInitResult = GOS_ERROR;
	}

	return ledInitResult;
}

GOS_STATIC void_t app_ledTask (void_t)
{
	for (;;)
	{
		(void_t) drv_gpioTgglePin(IO_USER_LED);
		(void_t) gos_taskSleep(LED_TASK_PERIOD);
	}
}
