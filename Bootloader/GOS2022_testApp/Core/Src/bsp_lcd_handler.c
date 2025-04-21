/*
 * bsp_lcd_handler.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Ahmed
 */

#include "bsp_lcd_handler.h"
#include "gos_lib.h"
#include <string.h>
#include <stdio.h>

#define LCD_MAX_LINE_LENGTH	( 128u )

GOS_STATIC char_t firstLine  [LCD_MAX_LINE_LENGTH];
GOS_STATIC char_t secondLine [LCD_MAX_LINE_LENGTH];
GOS_STATIC char_t firstLineTempBuffer [LCD_MAX_LINE_LENGTH];
GOS_STATIC char_t secondLineTempBuffer [LCD_MAX_LINE_LENGTH];
GOS_STATIC lcd_display_cfg_t firstLineConfig;
GOS_STATIC lcd_display_cfg_t secondLineConfig;
GOS_STATIC gos_mutex_t lcdMutex;
GOS_STATIC u16_t firstLineShiftCounter;
GOS_STATIC u16_t secondLineShiftCounter;

GOS_STATIC void_t       bsp_lcdHandlerTask               (void_t);
GOS_STATIC void_t       bsp_lcdHandlerShiftReset         (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction);
GOS_STATIC void_t       bsp_lcdHandlerShiftBuffer        (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction);
GOS_STATIC gos_result_t bsp_lcdHandlerWriteStringWrapper (u8_t params, va_list args);

GOS_STATIC gos_taskDescriptor_t lcdHandlerTaskDesc =
{
	.taskName           = "bsp_lcd_handler_task",
	.taskFunction       = bsp_lcdHandlerTask,
	.taskStackSize      = 0x400,
	.taskPriority       = 13,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

drv_lcdDescriptor_t lcdDescriptor =
{
	.deviceAddress   = 0x22,
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.writeMutexTmo   = GOS_MUTEX_ENDLESS_TMO,
	.writeTriggerTmo = 100
};

/**
 * Standard device descriptor for LCD.
 */
GOS_STATIC svl_dhsDevice_t lcdDevice =
{
	.name              = "lcd",
	.description       = "16x2 LCD over I2C. Address: 0x22.",
	.pInitializer      = drv_lcdInit,
	.pDeviceDescriptor = (void_t*)&lcdDescriptor,
	.writeFunctions[0] = bsp_lcdHandlerWriteStringWrapper,
	.enabled           = GOS_TRUE,
	.errorTolerance    = 100
};

/*
 * Function: bsp_lcdHandlerInit
 */
gos_result_t bsp_lcdHandlerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	//GOS_CONCAT_RESULT(initResult, drv_lcdInit((void_t*)&lcdDescriptor));
	GOS_CONCAT_RESULT(initResult, svl_dhsRegisterDevice(&lcdDevice));
	GOS_CONCAT_RESULT(initResult, svl_dhsForceInitialize(lcdDevice.deviceId));
	GOS_CONCAT_RESULT(initResult, gos_mutexInit(&lcdMutex));
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&lcdHandlerTaskDesc, NULL));

	return initResult;
}

/*
 * Function: bsp_lcdHandlerrDisplayText
 */
gos_result_t bsp_lcdHandlerrDisplayText (lcd_display_cfg_t* config,  const char_t* text, ...)
{
	/*
	 * Local variables.
	 */
	gos_result_t displayTextResult = GOS_ERROR;
	va_list args;

	/*
	 * Function code.
	 */
	if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
	{
		if ((config != NULL) && (text != NULL) &&
			(config->line < 2) && (config->blinkStartIndex < 16) &&
			(config->blinkEndIndex < 16) && (config->blinkStartIndex < config->blinkEndIndex))
		{
			va_start(args, text);

			if (config->line == 0)
			{
				memcpy((void*)&firstLineConfig, (const void*)config, sizeof(*config));
				vsprintf(firstLine, text, args);
				firstLineShiftCounter = 0u;
				if (firstLineConfig.displayMode == LCD_DISPLAY_NORMAL)
				{
					//drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
					svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);
				}
			}
			else
			{
				memcpy((void*)&secondLineConfig, (const void*)config, sizeof(*config));
				vsprintf(secondLine, text, args);
				secondLineShiftCounter = 0u;
				if (secondLineConfig.displayMode == LCD_DISPLAY_NORMAL)
				{
					//drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
					svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine);
				}
			}
			va_end(args);

			displayTextResult = GOS_SUCCESS;
		}
		else
		{
			// Config error.
		}

		(void_t) gos_mutexUnlock(&lcdMutex);
	}
	else
	{
		// Error.
	}

	return displayTextResult;
}

/*
 * Function: bsp_lcdHandlerWriteNextString
 */
gos_result_t bsp_lcdHandlerWriteNextString (u8_t line, char_t* str)
{
	/*
	 * Local variables.
	 */
	gos_result_t writeStringResult = GOS_ERROR;
	u8_t lineLength = 0u;

	/*
	 * Function code.
	 */
	if (line == 0)
	{
		lineLength = strlen(firstLine);
		strcpy((char_t*)(firstLine + lineLength), str);
		writeStringResult = svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);//drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
	}
	else if (line == 1)
	{
		lineLength = strlen(secondLine);
		strcpy((char_t*)(secondLine + lineLength), str);
		writeStringResult = svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine); //drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
	}

	return writeStringResult;
}

/**
 * TODO
 */
GOS_STATIC void_t bsp_lcdHandlerTask (void_t)
{
	for (;;)
	{
		switch (firstLineConfig.displayMode)
		{
			case LCD_DISPLAY_NORMAL:
			{
				break;
			}
			case LCD_DISPLAY_BLINK:
			{
				if ((gos_kernelGetSysTicks() - firstLineConfig.lastTick) > firstLineConfig.periodMs)
				{
					firstLineConfig.lastTick = gos_kernelGetSysTicks();
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						if (firstLineConfig.blinkState)
						{
							strcpy(firstLineTempBuffer, firstLine);

							for (u8_t index = firstLineConfig.blinkStartIndex; index < firstLineConfig.blinkEndIndex; index++)
							{
								firstLine[index] = ' ';
							}

							firstLineConfig.blinkState = !firstLineConfig.blinkState;
						}
						else
						{
							strcpy(firstLine, firstLineTempBuffer);
							firstLineConfig.blinkState = !firstLineConfig.blinkState;
						}

						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_CYCLIC_RIGHT:
			case LCD_DISPLAY_CYCLIC_LEFT:
			{
				if ((gos_kernelGetSysTicks() - firstLineConfig.lastTick) > firstLineConfig.periodMs)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						firstLineConfig.lastTick = gos_kernelGetSysTicks();
						bsp_lcdHandlerShiftBuffer(firstLine, &firstLineShiftCounter, firstLineConfig.displayMode);
						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_RIGHT:
			case LCD_DISPLAY_SHIFT_LEFT:
			{
				if ((gos_kernelGetSysTicks() - firstLineConfig.lastTick) > firstLineConfig.periodMs)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						firstLineConfig.lastTick = gos_kernelGetSysTicks();

						if (firstLineShiftCounter == strlen(firstLine) - 16u)
						{
							firstLineConfig.originalDisplayMode = firstLineConfig.displayMode;
							firstLineConfig.displayMode = LCD_DISPLAY_SHIFT_WAIT_1500;
						}
						else
						{
							bsp_lcdHandlerShiftBuffer(firstLine, &firstLineShiftCounter, firstLineConfig.displayMode);
							//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
							(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);
						}
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_WAIT_1500:
			{
				if ((gos_kernelGetSysTicks() - firstLineConfig.lastTick) > 1500)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						firstLineConfig.lastTick = gos_kernelGetSysTicks();
						bsp_lcdHandlerShiftReset(firstLine, &firstLineShiftCounter, firstLineConfig.originalDisplayMode);
						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 0, firstLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 0, firstLine);
						firstLineConfig.displayMode = LCD_DISPLAY_SHIFT_WAIT_500;
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_WAIT_500:
			{
				if ((gos_kernelGetSysTicks() - firstLineConfig.lastTick) > 500)
				{
					firstLineConfig.lastTick = gos_kernelGetSysTicks();
					firstLineConfig.displayMode = firstLineConfig.originalDisplayMode;
				}
				break;
			}
			default: break;
		}

		switch (secondLineConfig.displayMode)
		{
			case LCD_DISPLAY_NORMAL:
			{
				break;
			}
			case LCD_DISPLAY_BLINK:
			{
				if ((gos_kernelGetSysTicks() - secondLineConfig.lastTick) > secondLineConfig.periodMs)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						secondLineConfig.lastTick = gos_kernelGetSysTicks();

						if (secondLineConfig.blinkState)
						{
							strcpy(secondLineTempBuffer, secondLine);

							for (u8_t index = secondLineConfig.blinkStartIndex; index < secondLineConfig.blinkEndIndex; index++)
							{
								secondLine[index] = ' ';
							}

							secondLineConfig.blinkState = !secondLineConfig.blinkState;
						}
						else
						{
							strcpy(secondLine, secondLineTempBuffer);
							secondLineConfig.blinkState = !secondLineConfig.blinkState;
						}

						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine);
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_CYCLIC_RIGHT:
			case LCD_DISPLAY_CYCLIC_LEFT:
			{
				if ((gos_kernelGetSysTicks() - secondLineConfig.lastTick) > secondLineConfig.periodMs)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						secondLineConfig.lastTick = gos_kernelGetSysTicks();
						bsp_lcdHandlerShiftBuffer(secondLine, &secondLineShiftCounter, secondLineConfig.displayMode);
						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine);
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_RIGHT:
			case LCD_DISPLAY_SHIFT_LEFT:
			{
				if ((gos_kernelGetSysTicks() - secondLineConfig.lastTick) > secondLineConfig.periodMs)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						secondLineConfig.lastTick = gos_kernelGetSysTicks();

						if (secondLineShiftCounter == strlen(secondLine) - 16u)
						{
							secondLineConfig.originalDisplayMode = secondLineConfig.displayMode;
							secondLineConfig.displayMode = LCD_DISPLAY_SHIFT_WAIT_1500;
						}
						else
						{
							bsp_lcdHandlerShiftBuffer(secondLine, &secondLineShiftCounter, secondLineConfig.displayMode);
							//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
							(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine);
						}
						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_WAIT_1500:
			{
				if ((gos_kernelGetSysTicks() - secondLineConfig.lastTick) > 1500)
				{
					if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						secondLineConfig.lastTick = gos_kernelGetSysTicks();
						bsp_lcdHandlerShiftReset(secondLine, &secondLineShiftCounter, secondLineConfig.originalDisplayMode);
						//(void_t) drv_lcdWriteString((void_t*)&lcdDescriptor, 1, secondLine);
						(void_t) svl_dhsWriteDevice(lcdDevice.deviceId, 0, 3, (void_t*)&lcdDescriptor, 1, secondLine);
						secondLineConfig.displayMode = LCD_DISPLAY_SHIFT_WAIT_500;

						(void_t) gos_mutexUnlock(&lcdMutex);
					}
				}
				break;
			}
			case LCD_DISPLAY_SHIFT_WAIT_500:
			{
				if ((gos_kernelGetSysTicks() - secondLineConfig.lastTick) > 500)
				{
					secondLineConfig.lastTick = gos_kernelGetSysTicks();
					secondLineConfig.displayMode = secondLineConfig.originalDisplayMode;
				}
				break;
			}
			default: break;
		}
		gos_taskSleep(10);
	}
}

GOS_STATIC void_t bsp_lcdHandlerShiftReset (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction)
{
	u16_t index = 0u;
	u16_t cntr  = 0u;
	u16_t resetCycles = *pShiftCounter;
	switch (direction)
	{
		case LCD_DISPLAY_CYCLIC_LEFT:
		case LCD_DISPLAY_SHIFT_LEFT:
		{
			for (index = 0u; index < resetCycles; index++)
			{
				bsp_lcdHandlerShiftBuffer(pBuffer, &cntr, LCD_DISPLAY_SHIFT_RIGHT);
			}
			*pShiftCounter = 0U;
			break;
		}
		case LCD_DISPLAY_CYCLIC_RIGHT:
		case LCD_DISPLAY_SHIFT_RIGHT:
		{
			for (index = 0u; index < resetCycles; index++)
			{
				bsp_lcdHandlerShiftBuffer(pBuffer, &cntr, LCD_DISPLAY_SHIFT_LEFT);
			}
			*pShiftCounter = 0U;
			break;
		}
		default: break;
	}
}

GOS_STATIC void_t bsp_lcdHandlerShiftBuffer (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction)
{
	u16_t index = 0u;

	switch (direction)
	{
		case LCD_DISPLAY_CYCLIC_LEFT:
		case LCD_DISPLAY_SHIFT_LEFT:
		{
			char_t firstChar = pBuffer[0];
			for (index = 1u; index < strlen(pBuffer); index++)
			{
				pBuffer[index - 1] = pBuffer[index];
			}
			pBuffer[strlen(pBuffer) - 1] = firstChar;
			(*pShiftCounter)++;
			break;
		}
		case LCD_DISPLAY_CYCLIC_RIGHT:
		case LCD_DISPLAY_SHIFT_RIGHT:
		{
			u16_t length = (u8_t)strlen(pBuffer);
			char_t lastChar = pBuffer[strlen(pBuffer) - 1];

			for (index = 1u; index < strlen(pBuffer); index++)
			{
				pBuffer[length - index] = pBuffer[length - index - 1];
			}
			pBuffer[0] = lastChar;
			(*pShiftCounter)++;
			break;
		}
		default: break;
	}
}

// TODO
GOS_STATIC gos_result_t bsp_lcdHandlerWriteStringWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* pDevice;
	u8_t    line;
	const char_t* str;

	/*
	 * Function code.
	 */
	pDevice = (void_t*)va_arg(args, int);
	line = (u8_t)va_arg(args, int);
	str = (const char_t*)va_arg(args, int);

	return drv_lcdWriteString(pDevice, line, str);
}
