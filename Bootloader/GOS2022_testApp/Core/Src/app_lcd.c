/*
 * app_lcd.c
 *
 *  Created on: Jan 30, 2025
 *      Author: Ahmed
 */
#include "app.h"
#include "bsp_lcd_handler.h"
#include "mdi_def.h"
#include "gos_lib.h"

GOS_EXTERN app_button_block_t buttonBlock;
GOS_EXTERN svl_mdiVariable_t mdiVariables [];
GOS_EXTERN gos_mutex_t mdiMutex;

typedef enum
{
	PROGRAM_INIT_LOG,
	PROGRAM_MAIN_IDLE,
	PROGRAM_TIME_PRINT,
	PROGRAM_FLASHING_TEXT,
	PROGRAM_SHIFTING_TEXT,
	PROGRAM_TEMP_DISPLAY_INIT,
	PROGRAM_TEMP_VALUE_DISPLAY,
	PROGRAM_CPU_USAGE_DISPLAY_INIT,
	PROGRAM_CPU_USAGE_VALUE_DISPLAY,

	PROGRAM_STATE_MAX_NUM
}program_state_t;

GOS_STATIC program_state_t programState = PROGRAM_INIT_LOG;
GOS_STATIC gos_time_t actualTime;

void_t gpio_cfgUserButtonCallback (void_t);
GOS_STATIC void_t app_lcdTask (void_t);

GOS_STATIC gos_taskDescriptor_t appLcdTask =
{
	.taskFunction	= app_lcdTask,
	.taskName 		= "app_lcd_task",
	.taskStackSize 	= 0x600,
	.taskPriority 	= 14,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER | GOS_PRIV_TASK_MANIPULATE
};

gos_result_t app_lcdInit (void_t)
{
	gos_result_t lcdInitResult = GOS_SUCCESS;

	lcdInitResult &= gos_taskRegister(&appLcdTask, NULL);

	if (lcdInitResult != GOS_SUCCESS)
	{
		lcdInitResult = GOS_ERROR;
	}

	return lcdInitResult;
}

GOS_STATIC void_t app_lcdTask (void_t)
{
	/*
	 * Local variables.
	 */
	lcd_display_cfg_t flashingConfig =
	{
		.blinkStartIndex = 0u,
		.blinkEndIndex   = 15u,
		.displayMode     = LCD_DISPLAY_BLINK,
		.blinkState      = LCD_BLINK_STATE_OFF,
		.line            = 0,
		.periodMs        = 500
	};

	lcd_display_cfg_t normalConfig =
	{
		.blinkStartIndex = 0u,
		.blinkEndIndex   = 15u,
		.displayMode     = LCD_DISPLAY_NORMAL,
		.blinkState      = LCD_BLINK_STATE_OFF,
		.line            = 0,
		.periodMs        = 0
	};

	lcd_display_cfg_t shiftConfig =
	{
		.blinkStartIndex = 0u,
		.blinkEndIndex   = 15u,
		.displayMode     = LCD_DISPLAY_SHIFT_LEFT,
		.blinkState      = LCD_BLINK_STATE_OFF,
		.line            = 0,
		.periodMs        = 100
	};

	//u16_t adcValue         = 0u;
	u16_t temperatureValue = 0u;
	u16_t cpuUse           = 0u;

	(void_t) bsp_lcdHandlerInit();

	for (;;)
	{
		switch (programState)
		{
			case PROGRAM_INIT_LOG:
			{
				programState = PROGRAM_MAIN_IDLE;

				bsp_lcdHandlerrDisplayText(&normalConfig, "Hello from GOS!");

				for (u8_t i = 0; i < 16; i++)
				{
					bsp_lcdHandlerWriteNextString(1, "#");
					gos_taskSleep(200);
				}

				gos_taskSleep(1000);
				break;
			}
			case PROGRAM_MAIN_IDLE:
			{
				normalConfig.line = 0;
				bsp_lcdHandlerrDisplayText(&normalConfig, "GOS2022 test    ");
				normalConfig.line = 1;
				bsp_lcdHandlerrDisplayText(&normalConfig, "project         ");

				gos_taskSuspend(appLcdTask.taskId);
				break;
			}
			case PROGRAM_TIME_PRINT:
			{
				gos_timeGet(&actualTime);
				normalConfig.line = 0;
				bsp_lcdHandlerrDisplayText(&normalConfig, "%4d/%02d/%02d      ",
						actualTime.years,
						actualTime.months,
						actualTime.days
						);
				normalConfig.line = 1;

				bsp_lcdHandlerrDisplayText(&normalConfig, "%02d:%02d:%02d        ",
						actualTime.hours,
						actualTime.minutes,
						actualTime.seconds
						);

				gos_taskSleep(250);
				break;
			}
			case PROGRAM_FLASHING_TEXT:
			{
				flashingConfig.line = 0;
				flashingConfig.periodMs = 500;
				flashingConfig.blinkStartIndex = 1;
				flashingConfig.blinkEndIndex = 9;

				bsp_lcdHandlerrDisplayText(&flashingConfig, " Flashing test! ");

				flashingConfig.line = 1;
				flashingConfig.periodMs = 350;
				flashingConfig.blinkStartIndex = 0;
				flashingConfig.blinkEndIndex = 15;

				bsp_lcdHandlerrDisplayText(&flashingConfig, "    GOS2022     ");

				gos_taskSuspend(appLcdTask.taskId);
				break;
			}
			case PROGRAM_SHIFTING_TEXT:
			{
				shiftConfig.line = 0;
				shiftConfig.periodMs = 100;
				shiftConfig.displayMode = LCD_DISPLAY_SHIFT_LEFT;
				bsp_lcdHandlerrDisplayText(&shiftConfig, "This is a normal shifting text example.");

				shiftConfig.line = 1;
				shiftConfig.periodMs = 400;
				shiftConfig.displayMode = LCD_DISPLAY_CYCLIC_LEFT;
				bsp_lcdHandlerrDisplayText(&shiftConfig, "This is a cyclic shifting text example. The text is rotating continuously. ");

				gos_taskSuspend(appLcdTask.taskId);
				break;
			}
			/*case PROGRAM_ADC_DISPLAY_INIT:
			{
				shiftConfig.line = 1;
				shiftConfig.periodMs = 300;
				shiftConfig.displayMode = LCD_DISPLAY_SHIFT_LEFT;
				BSP_LCD_HandlerDisplayText(&shiftConfig, "The ADC driver is under development.");

				programState = PROGRAM_ADC_VALUE_DISPLAY;
				break;
			}
			case PROGRAM_ADC_VALUE_DISPLAY:
			{
				normalConfig.line = 0;
				adcValue = 0u;
				BSP_ADC_HandlerGetValue(&adcValue);
				BSP_LCD_HandlerDisplayText(&normalConfig, "ADC value: %u.%02uV   ", ((330 * adcValue / 4096) / 100), ((330 * adcValue / 4096) % 100));

				gos_taskSleep(200);
				break;
			}*/
			case PROGRAM_TEMP_DISPLAY_INIT:
			{
				shiftConfig.line = 1;
				shiftConfig.periodMs = 300;
				shiftConfig.displayMode = LCD_DISPLAY_SHIFT_LEFT;
				bsp_lcdHandlerrDisplayText(&shiftConfig, "The temperature shown here is CPU temperature.");

				programState = PROGRAM_TEMP_VALUE_DISPLAY;
				break;
			}
			case PROGRAM_TEMP_VALUE_DISPLAY:
			{
				normalConfig.line = 0;
				if (gos_mutexLock(&mdiMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
				{
					temperatureValue = (u16_t)(mdiVariables[MDI_CPU_TEMP].value.floatValue * 10);

					(void_t) gos_mutexUnlock(&mdiMutex);
				}
				bsp_lcdHandlerrDisplayText(&normalConfig, "Temp.: %u.%u C   ", (temperatureValue / 10), (temperatureValue % 10));

				gos_taskSleep(1000);
				break;
			}
			case PROGRAM_CPU_USAGE_DISPLAY_INIT:
			{
				shiftConfig.line = 1;
				shiftConfig.periodMs = 300;
				shiftConfig.displayMode = LCD_DISPLAY_SHIFT_LEFT;
				bsp_lcdHandlerrDisplayText(&shiftConfig, "Overall system CPU usage at the moment.");

				programState = PROGRAM_CPU_USAGE_VALUE_DISPLAY;
				break;
			}
			case PROGRAM_CPU_USAGE_VALUE_DISPLAY:
			{
				normalConfig.line = 0;
				cpuUse = gos_kernelGetCpuUsage();
				bsp_lcdHandlerrDisplayText(&normalConfig, "CPU: %u.%02u %%   ", (cpuUse / 100), (cpuUse % 100));
				gos_taskSleep(250);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void_t gpio_cfgUserButtonCallback (void_t)
{
	/*
	 * Function code.
	 */
	/* Interrupt is privileged */
	if (buttonBlock.buttonXBlocked[APP_USER_BUTTON] == GOS_FALSE)
	{
		buttonBlock.buttonXBlocked[APP_USER_BUTTON] = GOS_TRUE;

		if (++programState == PROGRAM_STATE_MAX_NUM)
		{
			programState = PROGRAM_MAIN_IDLE;
		}

		gos_taskUnblock(appLcdTask.taskId);
		gos_taskWakeup(appLcdTask.taskId);
		gos_taskResume(appLcdTask.taskId);

		(void_t) app_debouncerActivate();
		//APP_DAC_PlayBeep();
	}
}
