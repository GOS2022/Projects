/*
 * bsp_lcd_handler.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Gabor
 */

#include "bsp_lcd_handler.h"
#include "lcd_driver.h"
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

GOS_STATIC void_t BSP_LCD_HandlerTask (void_t);
GOS_STATIC void_t BSP_LCD_HandlerShiftReset (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction);
GOS_STATIC void_t BSP_LCD_HandlerShiftBuffer (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction);

GOS_STATIC gos_taskDescriptor_t lcdHandlerTaskDesc =
{
	.taskName = "bsp_lcd_handler_task",
	.taskFunction = BSP_LCD_HandlerTask,
	.taskStackSize = 0x400,
	.taskPriority = 13,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

gos_result_t BSP_LCD_HandlerInit (void_t)
{
	gos_result_t initResult = GOS_SUCCESS;

	initResult &= lcd_driver_init();
	initResult &= gos_mutexInit(&lcdMutex);
	initResult &= gos_taskRegister(&lcdHandlerTaskDesc, NULL);

	if (initResult != GOS_SUCCESS)
	{
		initResult = GOS_ERROR;
	}

	return initResult;
}

gos_result_t BSP_LCD_HandlerDisplayText (lcd_display_cfg_t* config,  const char_t* text, ...)
{
	gos_result_t displayTextResult = GOS_ERROR;
	va_list args;

	if (gos_mutexLock(&lcdMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
	{
		if (config != NULL && text != NULL &&
			config->line < 3 && config->blinkStartIndex < 16 &&
			config->blinkEndIndex < 16 && config->blinkStartIndex < config->blinkEndIndex)
		{
			va_start(args, text);

			if (config->line == 0)
			{
				memcpy((void*)&firstLineConfig, (const void*)config, sizeof(*config));
				vsprintf(firstLine, text, args);
				firstLineShiftCounter = 0u;
				if (firstLineConfig.displayMode == LCD_DISPLAY_NORMAL)
					lcd_driver_write_string(0, firstLine);
			}
			else
			{
				memcpy((void*)&secondLineConfig, (const void*)config, sizeof(*config));
				vsprintf(secondLine, text, args);
				secondLineShiftCounter = 0u;
				if (secondLineConfig.displayMode == LCD_DISPLAY_NORMAL)
					lcd_driver_write_string(1, secondLine);
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

gos_result_t BSP_LCD_HandlerWriteNextString (u8_t line, char_t* str)
{
	gos_result_t writeStringResult = GOS_ERROR;
	u8_t lineLength = 0u;

	if (line == 0)
	{
		lineLength = strlen(firstLine);
		strcpy((char_t*)(firstLine + lineLength), str);
		writeStringResult = lcd_driver_write_string(0, firstLine);
	}
	else if (line == 1)
	{
		lineLength = strlen(secondLine);
		strcpy((char_t*)(secondLine + lineLength), str);

		writeStringResult = lcd_driver_write_string(1, secondLine);
	}

	return writeStringResult;
}

GOS_STATIC void_t BSP_LCD_HandlerTask (void_t)
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

						lcd_driver_write_string(0, firstLine);
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
						BSP_LCD_HandlerShiftBuffer(firstLine, &firstLineShiftCounter, firstLineConfig.displayMode);
						lcd_driver_write_string(0, firstLine);
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
							BSP_LCD_HandlerShiftBuffer(firstLine, &firstLineShiftCounter, firstLineConfig.displayMode);
							lcd_driver_write_string(0, firstLine);
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
						BSP_LCD_HandlerShiftReset(firstLine, &firstLineShiftCounter, firstLineConfig.originalDisplayMode);
						lcd_driver_write_string(0, firstLine);
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

						lcd_driver_write_string(1, secondLine);

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
						BSP_LCD_HandlerShiftBuffer(secondLine, &secondLineShiftCounter, secondLineConfig.displayMode);
						lcd_driver_write_string(1, secondLine);
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
							BSP_LCD_HandlerShiftBuffer(secondLine, &secondLineShiftCounter, secondLineConfig.displayMode);
							lcd_driver_write_string(1, secondLine);
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
						BSP_LCD_HandlerShiftReset(secondLine, &secondLineShiftCounter, secondLineConfig.originalDisplayMode);
						lcd_driver_write_string(1, secondLine);
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

GOS_STATIC void_t BSP_LCD_HandlerShiftReset (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction)
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
				BSP_LCD_HandlerShiftBuffer(pBuffer, &cntr, LCD_DISPLAY_SHIFT_RIGHT);
			}
			*pShiftCounter = 0U;
			break;
		}
		case LCD_DISPLAY_CYCLIC_RIGHT:
		case LCD_DISPLAY_SHIFT_RIGHT:
		{
			for (index = 0u; index < resetCycles; index++)
			{
				BSP_LCD_HandlerShiftBuffer(pBuffer, &cntr, LCD_DISPLAY_SHIFT_LEFT);
			}
			*pShiftCounter = 0U;
			break;
		}
		default: break;
	}
}

GOS_STATIC void_t BSP_LCD_HandlerShiftBuffer (char_t* pBuffer, u16_t* pShiftCounter, lcd_display_mode_t direction)
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
