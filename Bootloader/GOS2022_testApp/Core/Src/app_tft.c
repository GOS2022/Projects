/*
 * app_tft.c
 *
 *  Created on: Dec 25, 2022
 *      Author: Ahmed
 */

#include "app.h"
#include "ili9341.h"
#include "ili9341_gfx.h"
#include <stdio.h>
#include "g_driver.h"
#include "mdi_def.h"
#include "gos_lib.h"

#define CPU_LOAD_AVG_SAMPLES ( 16u )

#define TEXT_SW_VER    "Software version: "
#define TEXT_SW_DATE   "Software date:    "
#define TEXT_ACT_TIME  "Actual time:      "
#define TEXT_RUNTIME   "System runtime:   "
#define TEXT_TEMP      "CPU temperature:  "
#define TEXT_CPU_LOAD  "CPU load:         "

typedef enum
{
	WINDOW_STATE_STATUS,
	WINDOW_STATE_MONITORING
}app_tftWindowState_t;

GOS_EXTERN svl_mdiVariable_t mdiVariables [];
GOS_EXTERN gos_mutex_t mdiMutex;

GOS_STATIC void_t APP_TFT_Task (void_t);
//GOS_STATIC void_t APP_TFT_ExIoPressed (void_t);
//GOS_STATIC void_t APP_TFT_OkButtonPressed (void_t);
//GOS_STATIC void_t APP_TFT_OkButtonReleased (void_t);
GOS_STATIC void_t APP_TFT_PopUpWindowOkClick (g_button_t* pButton);
GOS_STATIC void_t APP_TFT_PopUpWindowResetClick (g_button_t* pButton);

GOS_STATIC void_t app_tftStepButtonPressed (void_t);

GOS_STATIC g_button_t okButton =
{
	.text = "OK",
	.width = 40,
	.height = 20,
	.xTop = 100 + 150/3 - 40/2,
	.yTop = 100 + 70,
	.isEnabled = GOS_TRUE,
	.isVisible = GOS_TRUE,
	.isFocused = GOS_FALSE,
	.tabStop = GOS_TRUE,
	.tabIndex = 0u,
	.released = APP_TFT_PopUpWindowOkClick
};

GOS_STATIC g_button_t resetButton =
{
	.text = "RESET",
	.width = 50,
	.height = 20,
	.xTop = 100 + 2*150/3 - 50/2,
	.yTop = 100 + 70,
	.isEnabled = GOS_TRUE,
	.isVisible = GOS_TRUE,
	.isFocused = GOS_FALSE,
	.tabStop = GOS_TRUE,
	.tabIndex = 1u,
	.released = APP_TFT_PopUpWindowResetClick
};

GOS_STATIC g_label_t label =
{
	.text = "X button pressed!",
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 100 + 10,
	.yTop = 100 + 30
};

GOS_STATIC g_window_t popupWindow =
{
	.title = "Warning",
	.width = 150,
	.height = 75,
	.xTop = 100,
	.yTop = 100,
	.isVisible = GOS_FALSE,
	.isEnabled = GOS_TRUE
};

GOS_STATIC g_window_t statusWindow =
{
	.title = "System Status",
	.width = 480,
	.height = 320,
	.xTop = 0,
	.yTop = 0,
	.isVisible = GOS_FALSE,
	.isEnabled = GOS_TRUE,

};

GOS_STATIC g_window_t monitoringWindow =
{
	.title = "System Monitoring",
	.width = 480,
	.height = 320,
	.xTop = 0,
	.yTop = 0,
	.isVisible = GOS_TRUE,
	.isEnabled = GOS_TRUE
};

GOS_STATIC g_label_t projVerLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t projDateLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t timeLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t upTimeLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t tempLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t cpuLabel =
{
	.width = LABEL_AUTO_WIDTH,
	.height = LABEL_AUTO_HEIGHT,
	.xTop = 10
};

GOS_STATIC g_label_t popWindowTitleLabel, statusWindowTitleLabel, monitoringWindowTitleLabel;
GOS_STATIC g_button_t popWindowCloseButton, statusWindowCloseButton, monitoringWindowCloseButton;
GOS_STATIC char_t labelText [LABEL_TEXT_MAX_LENGTH];
GOS_STATIC gos_time_t actualTime;
GOS_STATIC gos_runtime_t runTime;
GOS_STATIC u16_t temperatureValue;
GOS_STATIC u16_t cpuLoad;
GOS_STATIC u16_t cpuLoadArray [128];
GOS_STATIC u16_t cpuLoadIdx = 0u;
GOS_STATIC app_tftWindowState_t windowState = WINDOW_STATE_MONITORING;

/**
 * Labels for tasks.
 */
GOS_STATIC g_label_t monitoringHeaderLabels [] =
{
	{
		.width  = LABEL_AUTO_WIDTH,
		.height = LABEL_AUTO_HEIGHT,
		.xTop = 10,
		.yTop = 20 + 10,
		.text = "Task ID"
	},
	{
		.width  = LABEL_AUTO_WIDTH,
		.height = LABEL_AUTO_HEIGHT,
		.xTop = 75,
		.yTop = 20 + 10,
		.text = "Task name"
	},
	{
		.width  = LABEL_AUTO_WIDTH,
		.height = LABEL_AUTO_HEIGHT,
		.xTop = 225,
		.yTop = 20 + 10,
		.text = "CPU load"
	}
};

GOS_STATIC g_label_t monitoringTaskNameLabels [CFG_TASK_MAX_NUMBER];
GOS_STATIC g_label_t taskIdLabels [CFG_TASK_MAX_NUMBER];
GOS_STATIC g_label_t cpuUsageLabels [CFG_TASK_MAX_NUMBER];

GOS_STATIC gos_taskDescriptor_t tftTaskDesc =
{
	.taskFunction	    = APP_TFT_Task,
	.taskName		    = "app_tft_task",
	.taskStackSize 	    = 0x800,
	.taskPriority 	    = 80,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

gos_result_t app_tftInit (void_t)
{
	gos_result_t tftInitResult = GOS_SUCCESS;

	tftInitResult &= gos_taskRegister(&tftTaskDesc, NULL);

	if (tftInitResult != GOS_SUCCESS)
	{
		tftInitResult = GOS_ERROR;
	}

	return tftInitResult;
}


GOS_STATIC void_t APP_TFT_Task (void_t)
{
	/*
	 * Local variables.
	 */
	u32_t           lastTempTick = 0;
	u32_t           lastTimeTick = 0;
	u32_t           lastCpuTick  = 0;
	u32_t           lastMonitoringTick = 0;
	svl_pdhSwInfo_t swInfo       = {0};

	/*
	 * Function code.
	 */
	label.backColor = ILI9341_WHITE;
	label.foreColor = ILI9341_BLACK;

	okButton.backColor = ILI9341_WHITE;
	okButton.foreColor = ILI9341_BLACK;

	resetButton.backColor = ILI9341_WHITE;
	resetButton.foreColor = ILI9341_BLACK;

	projVerLabel.backColor = ILI9341_OLIVE;
	projVerLabel.foreColor = ILI9341_BLACK;
	projVerLabel.yTop = 20 + 10;

	projDateLabel.backColor = ILI9341_OLIVE;
	projDateLabel.foreColor = ILI9341_BLACK;
	projDateLabel.yTop = projVerLabel.yTop + 15;

	timeLabel.backColor = ILI9341_OLIVE;
	timeLabel.foreColor = ILI9341_BLACK;
	timeLabel.yTop = projDateLabel.yTop + 15;

	upTimeLabel.backColor = ILI9341_OLIVE;
	upTimeLabel.foreColor = ILI9341_BLACK;
	upTimeLabel.yTop = timeLabel.yTop + 15;

	tempLabel.backColor = ILI9341_OLIVE;
	tempLabel.foreColor = ILI9341_BLACK;
	tempLabel.yTop = upTimeLabel.yTop + 15;

	cpuLabel.backColor = ILI9341_OLIVE;
	cpuLabel.foreColor = ILI9341_BLACK;
	cpuLabel.yTop = tempLabel.yTop + 15;

	popupWindow.backColor = ILI9341_WHITE;
	statusWindow.backColor = ILI9341_OLIVE;
	monitoringWindow.backColor = ILI9341_WHITE;

	// Low-level initializations.
	(void_t) ili9341_init(isoLandscape, itsSupported, itnNormalized);
	(void_t) g_initGraphicsDriver();

	// Add buttons to popupWindow.
	g_windowAddButton(&popupWindow, &okButton);
	g_windowAddButton(&popupWindow, &resetButton);

	// Add label to popupWindow.
	g_windowAddLabel(&popupWindow, &label);

	// Register popupWindow.
	g_registerWindow(&popupWindow);

	// Add labels to status window.
	g_windowAddLabel(&statusWindow, &projVerLabel);
	g_windowAddLabel(&statusWindow, &projDateLabel);
	g_windowAddLabel(&statusWindow, &timeLabel);
	g_windowAddLabel(&statusWindow, &upTimeLabel);
	g_windowAddLabel(&statusWindow, &tempLabel);
	g_windowAddLabel(&statusWindow, &cpuLabel);

	// Register status window.
	g_registerWindow(&statusWindow);

	svl_pdhGetSwInfo(&swInfo);
	sprintf(labelText, TEXT_SW_VER"%02d.%02d.%02d",
			swInfo.appSwVerInfo.major,
			swInfo.appSwVerInfo.minor,
			swInfo.appSwVerInfo.build
		);
	g_labelSetText(&projVerLabel, labelText);

	sprintf(labelText, TEXT_SW_DATE"%04d-%02d-%02d",
			swInfo.appSwVerInfo.date.years,
			swInfo.appSwVerInfo.date.months,
			swInfo.appSwVerInfo.date.days
		);
	g_labelSetText(&projDateLabel, labelText);

	// Initialize monitoring window.
	gos_tid_t currentId;
	u16_t numOfTasks;
	u16_t yOffset = 20+10+15;
	gos_taskDescriptor_t taskData;

	monitoringHeaderLabels[0].backColor = ILI9341_WHITE;
	monitoringHeaderLabels[0].foreColor = ILI9341_BLACK;

	monitoringHeaderLabels[1].backColor = ILI9341_WHITE;
	monitoringHeaderLabels[1].foreColor = ILI9341_BLACK;

	monitoringHeaderLabels[2].backColor = ILI9341_WHITE;
	monitoringHeaderLabels[2].foreColor = ILI9341_BLACK;

	(void_t) g_windowAddLabel(&monitoringWindow, &monitoringHeaderLabels[0]);
	(void_t) g_windowAddLabel(&monitoringWindow, &monitoringHeaderLabels[1]);
	(void_t) g_windowAddLabel(&monitoringWindow, &monitoringHeaderLabels[2]);

	(void_t) gos_taskGetNumber(&numOfTasks);
	(void_t) gos_taskGetCurrentId(&currentId);
	(void_t) gos_taskAddPrivilege(currentId, GOS_TASK_PRIVILEGE_KERNEL);

	for (u16_t i = 0; i < numOfTasks && yOffset < (320-15); i++)
	{
		(void_t) gos_taskGetDataByIndex(i, &taskData);

		taskIdLabels[i].width = LABEL_AUTO_WIDTH;
		taskIdLabels[i].height = LABEL_AUTO_HEIGHT;
		taskIdLabels[i].xTop = 10;
		taskIdLabels[i].yTop = yOffset;
		taskIdLabels[i].backColor = ILI9341_WHITE;
		taskIdLabels[i].foreColor = ILI9341_BLACK;
		sprintf(taskIdLabels[i].text, "0x%04x", taskData.taskId);
    	// Add label to window.
    	g_windowAddLabel(&monitoringWindow, &taskIdLabels[i]);

        monitoringTaskNameLabels[i].width  = LABEL_AUTO_WIDTH;
        monitoringTaskNameLabels[i].height = LABEL_AUTO_HEIGHT;
        monitoringTaskNameLabels[i].xTop = 75;
        monitoringTaskNameLabels[i].yTop = yOffset;
        monitoringTaskNameLabels[i].backColor = ILI9341_WHITE;
        monitoringTaskNameLabels[i].foreColor = ILI9341_BLACK;
        strcpy(monitoringTaskNameLabels[i].text, taskData.taskName);
    	// Add label to window.
    	g_windowAddLabel(&monitoringWindow, &monitoringTaskNameLabels[i]);

    	cpuUsageLabels[i].width = LABEL_AUTO_WIDTH;
    	cpuUsageLabels[i].height = LABEL_AUTO_HEIGHT;
    	cpuUsageLabels[i].xTop = 225;
    	cpuUsageLabels[i].yTop = yOffset;
    	cpuUsageLabels[i].backColor = ILI9341_WHITE;
    	cpuUsageLabels[i].foreColor = ILI9341_BLACK;
		sprintf(cpuUsageLabels[i].text, "%u.%02u %%  ", taskData.taskCpuUsage / 100, taskData.taskCpuUsage % 100);
    	// Add label to window.
    	g_windowAddLabel(&monitoringWindow, &cpuUsageLabels[i]);

        yOffset += 15;
	}

	(void_t) gos_taskRemovePrivilege(currentId, GOS_TASK_PRIVILEGE_KERNEL);

	// Register monitoring.
	g_registerWindow(&monitoringWindow);

	// Initialize windows.
	g_windowInit(&popupWindow, &popWindowCloseButton, &popWindowTitleLabel);
	g_windowInit(&statusWindow, &statusWindowCloseButton, &statusWindowTitleLabel);
	g_windowInit(&monitoringWindow, &monitoringWindowCloseButton, &monitoringWindowTitleLabel);

	bsp_exioRegisterCallback(GPIO_EX_PIN_4, GPIO_EX_EDGE_FALLING, app_tftStepButtonPressed);

	for (;;)
	{
		switch(windowState)
		{
			case WINDOW_STATE_MONITORING:
			{
				// TODO: update values.
				if ((gos_kernelGetSysTicks() - lastMonitoringTick) > 250)
				{
					(void_t) gos_taskAddPrivilege(currentId, GOS_TASK_PRIVILEGE_KERNEL);
					for (u16_t i = 0; i < numOfTasks; i++)
					{
						(void_t) gos_taskGetDataByIndex(i, &taskData);

						sprintf(labelText, "%u.%02u %%  ",
								(taskData.taskCpuUsage / 100),
								(taskData.taskCpuUsage % 100)
							);

						g_labelSetText(&cpuUsageLabels[i], labelText);
					}
					(void_t) gos_taskRemovePrivilege(currentId, GOS_TASK_PRIVILEGE_KERNEL);
					lastMonitoringTick = gos_kernelGetSysTicks();
				}
				break;
			}
			case WINDOW_STATE_STATUS:
			{
				/* TIME */
				if ((gos_kernelGetSysTicks() - lastTimeTick) > 250)
				{
					// Update time label.
					gos_timeGet(&actualTime);
					sprintf(labelText, TEXT_ACT_TIME"%04d-%02d-%02d %02d:%02d:%02d",
							actualTime.years,
							actualTime.months,
							actualTime.days,
							actualTime.hours,
							actualTime.minutes,
							actualTime.seconds
						);
					g_labelSetText(&timeLabel, labelText);

					// Update uptime label
					gos_runTimeGet(&runTime);
					sprintf(labelText, TEXT_RUNTIME"%02d days %02d:%02d:%02d",
							runTime.days,
							runTime.hours,
							runTime.minutes,
							runTime.seconds
						);
					g_labelSetText(&upTimeLabel, labelText);

					lastTimeTick = gos_kernelGetSysTicks();
				}

				/* TEMPERATURE */
				if ((gos_kernelGetSysTicks() - lastTempTick) > 1000)
				{
					// Update temperature label.
					//BSP_Temperature_HandlerGetValue(&temperatureValue);
					if (gos_mutexLock(&mdiMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
					{
						temperatureValue = (u16_t)(mdiVariables[MDI_CPU_TEMP].value.floatValue * 10);

						(void_t) gos_mutexUnlock(&mdiMutex);
					}

					sprintf(labelText, TEXT_TEMP"%u.%02u C  ",
							(temperatureValue / 10), (temperatureValue % 10)
						);
					g_labelSetText(&tempLabel, labelText);

					lastTempTick = gos_kernelGetSysTicks();
				}

				// Update CPU load label.
				cpuLoadArray[(cpuLoadIdx++ % CPU_LOAD_AVG_SAMPLES)] = gos_kernelGetCpuUsage();
				u32_t cpuSum = 0u;
				for (u8_t i = 0; i < CPU_LOAD_AVG_SAMPLES; i++)
				{
					cpuSum += cpuLoadArray[i];
				}
				cpuLoad = cpuSum / CPU_LOAD_AVG_SAMPLES;

				if ((gos_kernelGetSysTicks() - lastCpuTick) > 500)
				{
					sprintf(labelText, TEXT_CPU_LOAD"%u.%02u %%  ",
							(cpuLoad / 100), (cpuLoad % 100)
						);
					g_labelSetText(&cpuLabel, labelText);

					lastCpuTick = gos_kernelGetSysTicks();
				}
				break;
			}
		}

		(void_t) gos_taskSleep(100);
	}
}

GOS_STATIC void_t APP_TFT_PopUpWindowOkClick (g_button_t* pButton)
{
	g_windowHide(&popupWindow);
}

GOS_STATIC void_t APP_TFT_PopUpWindowResetClick (g_button_t* pButton)
{
	gos_kernelReset();
}

/*GOS_STATIC void_t APP_TFT_StepButtonPressed (void_t)
{
	g_windowTabStep(&popupWindow);
}

GOS_STATIC void_t APP_TFT_ExIoPressed (void_t)
{
	g_windowShow(&popupWindow);
}

GOS_STATIC void_t APP_TFT_OkButtonPressed (void_t)
{
	g_windowPressFocused(&popupWindow);
}

GOS_STATIC void_t APP_TFT_OkButtonReleased (void_t)
{
	g_windowReleaseFocused(&popupWindow);
}*/

GOS_STATIC void_t app_tftStepButtonPressed (void_t)
{
	if (statusWindow.isVisible == GOS_TRUE)
	{
		g_windowHide(&statusWindow);
		g_windowShow(&monitoringWindow);
		windowState = WINDOW_STATE_MONITORING;
	}
	else
	{
		g_windowHide(&monitoringWindow);
		g_windowShow(&statusWindow);
		windowState = WINDOW_STATE_STATUS;
	}
}
