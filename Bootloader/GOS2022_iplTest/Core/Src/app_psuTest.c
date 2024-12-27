/*
 * app_psuTest.c
 *
 *  Created on: Dec 20, 2024
 *      Author: Gabor
 */
#include "app.h"
#include "drv.h"

GOS_STATIC drv_mcp23017Descriptor_t psuController =
{
	.deviceAddress   = 0x27,
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.portADir        = MCP23017_IODIR_ALL_OUTPUT,
	.portBDir        = MCP23017_IODIR_ALL_OUTPUT,
	.portAPol        = MCP23017_IPOL_ALL_INVERTED,
	.portBPol        = MCP23017_IPOL_ALL_INVERTED,
	.readMutexTmo    = 1000u,
	.readTriggerTmo  = 1000u,
	.writeMutexTmo   = 1000u,
	.writeTriggerTmo = 1000u
};

GOS_STATIC void_t app_psuTestTask (void_t);

GOS_STATIC gos_taskDescriptor_t psuTestTask =
{
	.taskFunction	= app_psuTestTask,
	.taskName 		= "app_psuTestTask",
	.taskStackSize 	= 0x400,
	.taskPriority 	= 5,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGED_USER
};

gos_result_t app_psuTestInit (void_t)
{
	return gos_taskRegister(&psuTestTask, NULL);
}

GOS_STATIC void_t app_psuTestTask (void_t)
{
	(void_t) drv_mcp23017Init((void_t*)&psuController);
	(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_0, GPIO_STATE_HIGH);

	for (;;)
	{
		//(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_0, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_1, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_2, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_3, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_4, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_5, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_6, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_7, GPIO_STATE_LOW);

		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_0, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_1, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_2, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_3, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_4, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_5, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_6, GPIO_STATE_LOW);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_7, GPIO_STATE_LOW);

		(void_t) gos_taskSleep(2000);

		//(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_0, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_1, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_2, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_3, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_4, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_5, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_6, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTA, DRV_MCP23017_PIN_7, GPIO_STATE_HIGH);

		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_0, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_1, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_2, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_3, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_4, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_5, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_6, GPIO_STATE_HIGH);
		(void_t) drv_mcp23017WritePin(&psuController, MCP23017_PORTB, DRV_MCP23017_PIN_7, GPIO_STATE_HIGH);

		(void_t) gos_taskSleep(2000);
	}
}
