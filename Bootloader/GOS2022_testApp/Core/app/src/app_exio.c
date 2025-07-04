/*
 * app_exio.c
 *
 *  Created on: Apr 8, 2025
 *      Author: Ahmed
 */
#include "app.h"
#include "gos_lib.h"

GOS_STATIC u8_t pinStates = 0b10000000;
GOS_STATIC bool_t shiftRight = GOS_TRUE;

GOS_STATIC void_t app_exioTask (void_t);

gos_taskDescriptor_t exIoTaskDesc =
{
	.taskFunction       = app_exioTask,
	.taskName           = "app_exio_task",
	.taskStackSize      = 0x400,
	.taskPriority       = 8,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: app_exioInit
 */
gos_result_t app_exioInit (void_t)
{
	/*
	 * Function code.
	 */
	return gos_taskRegister(&exIoTaskDesc, NULL);
}

/**
 * TODO
 * @param
 * @return
 */
GOS_STATIC void_t app_exioTask (void_t)
{
	/*
	 * Function code.
	 */
	for (;;)
	{
		(void_t) bsp_exioWritePort(pinStates);

		if (shiftRight == GOS_TRUE)
		{
			switch (pinStates)
			{
				case 0b10000000:
				{
					pinStates = 0b01000000;
					break;
				}
				case 0b01000000:
				{
					pinStates = 0b00010000;
					break;
				}
				case 0b00010000:
				{
					pinStates = 0b00100000;
					break;
				}
				case 0b00100000:
				{
					pinStates = 0b00001000;
					break;
				}
				case 0b00001000:
				{
					pinStates = 0b00000100;
					break;
				}
				case 0b00000100:
				{
					pinStates = 0b00000010;
					break;
				}
				case 0b00000010:
				{
					pinStates = 0b00000001;
					shiftRight = GOS_FALSE;
					break;
				}
			}
		}
		else
		{
			switch (pinStates)
			{
				case 0b00000001:
				{
					pinStates = 0b00000010;
					break;
				}
				case 0b00000010:
				{
					pinStates = 0b00000100;
					break;
				}
				case 0b00000100:
				{
					pinStates = 0b00001000;
					break;
				}
				case 0b00001000:
				{
					pinStates = 0b00100000;
					break;
				}
				case 0b00100000:
				{
					pinStates = 0b00010000;
					break;
				}
				case 0b00010000:
				{
					pinStates = 0b01000000;
					break;
				}
				case 0b01000000:
				{
					pinStates = 0b10000000;
					shiftRight = GOS_TRUE;
					break;
				}
			}
		}

		(void_t) gos_taskSleep(50);
	}
}
