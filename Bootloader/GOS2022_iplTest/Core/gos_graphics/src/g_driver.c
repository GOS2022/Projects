/*
 * g_driver.c
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */
#include "g_driver.h"
#include "ili9341.h"
//#include "gpio_driver.h"
#include "iodef.h"
#include <string.h>

gos_tid_t g_driverTaskId;
gos_mutex_t g_mutex;

GOS_STATIC g_window_t* pWindows [G_DRIVER_WINDOW_MAX_NUM];

//GOS_STATIC ili9341_t* screen;

GOS_STATIC u8_t numOfWindows;

GOS_STATIC void_t g_task (void_t);
GOS_STATIC void_t g_redrawHandler (g_windowEvent_t event, gos_message_t* pEventMessage);
//GOS_STATIC void_t g_touchItCallback (void_t);
//GOS_STATIC void_t g_screenTouchBegin(ili9341_t *lcd, u16_t x, u16_t y);
//GOS_STATIC void_t g_screenTouchEnd(ili9341_t *lcd, u16_t x, u16_t y);

//GOS_STATIC void_t g_windowPressed (g_window_t* pWindow);
//GOS_STATIC void_t g_windowClicked (g_window_t* pWindow);


GOS_STATIC gos_taskDescriptor_t g_taskDescriptor =
{
	.taskFunction = g_task,
	.taskName     = "graphics_driver_task",
	.taskStackSize = 0x600,
	.taskPriority = 200,//85,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

gos_result_t g_initGraphicsDriver (/*ili9341_t* lcd*/)
{
	/*gpio_callback_desc_t touchInterruptDesc =
	{
		.callback = g_touchItCallback,
		.pin = iodef_get(IO_TOUCH_INT)->pin
	};*/

	//screen = lcd;
	numOfWindows = 0u;
	gos_mutexInit(&g_mutex);

	//drv_gpio_registerItCallback(&touchInterruptDesc);

	//ili9341_set_touch_pressed_begin(g_screenTouchBegin);
	//ili9341_set_touch_pressed_end(g_screenTouchEnd);

	return gos_taskRegister(&g_taskDescriptor, &g_driverTaskId);
}

gos_result_t g_registerWindow (g_window_t* pWindow)
{
	u16_t windowIndex = 0u;

	for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
	{
		if (pWindows[windowIndex] == NULL)
		{
			pWindows[windowIndex] = pWindow;
			pWindow->zIndex = 0u;
			numOfWindows++;
			return GOS_SUCCESS;
		}
		else
		{
			pWindows[windowIndex]->zIndex++;
		}
	}

	return GOS_ERROR;
}

void_t g_activateWindow (g_window_t* pWindow)
{
	u16_t windowIndex = 0;
	u16_t lastZIndex = pWindow->zIndex;

	for (windowIndex = 0; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
	{
		if (pWindows[windowIndex] != NULL)
		{
			if (pWindows[windowIndex]->zIndex < lastZIndex)
			{
				pWindows[windowIndex]->zIndex++;
			}
		}
		else
		{
			break;
		}
	}

	pWindow->zIndex = 0u;
}

void_t g_inactivateWindow (g_window_t* pWindow)
{
	u16_t windowIndex = 0;

	if (pWindow->zIndex == 0u)
	{
		for (windowIndex = 0; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
		{
			if (pWindows[windowIndex] != NULL)
			{
				pWindows[windowIndex]->zIndex--;
			}
			else
			{
				break;
			}
		}
	}

	pWindow->zIndex = numOfWindows - 1;
}

GOS_STATIC void_t g_task (void_t)
{
	gos_messageId_t selector [] = { WINDOW_EVENT_MSG_ID, 0 };
	gos_message_t eventMessage;
	ili9341_fill_screen(ILI9341_BLACK);

	// Draw at startup.
	g_redrawHandler(EVENT_STARTUP, &eventMessage);

	for (;;)
	{
		if (gos_messageRx(selector, &eventMessage, GOS_MESSAGE_ENDLESS_TMO) == GOS_SUCCESS)
		{
			g_redrawHandler((g_windowEvent_t)eventMessage.messageBytes[0], &eventMessage);
		}
	}
}

GOS_STATIC void_t g_redrawHandler (g_windowEvent_t event, gos_message_t* pEventMessage)
{
	u16_t windowIndex = 0u;
	u8_t idx, zIndex;
	bool_t breakLoop = GOS_FALSE;

	switch (event)
	{
	case EVENT_STARTUP:
	{
		// Redraw everything
		zIndex = numOfWindows - 1;
		for (idx = 0u; idx < numOfWindows; idx++)
		{
			for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
			{
				if (pWindows[windowIndex] == NULL)
				{
					break;
				}
				else
				{
					if (pWindows[windowIndex]->zIndex == zIndex)
					{
						if (pWindows[windowIndex]->isVisible == GOS_TRUE)
						{
							g_windowDraw(pWindows[windowIndex]);
						}
						break;
					}
				}
			}
			zIndex--;
		}
		break;
	}
	case EVENT_WINDOW_HIDDEN:
	{
		g_window_t * pWindow;
		memcpy((void_t*)&pWindow, (void_t*)&pEventMessage->messageBytes[1], sizeof(pWindow));
		GOS_NOP;

		zIndex = numOfWindows - 1;
		for (idx = 0u; idx < numOfWindows; idx++)
		{
			for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
			{
				if (pWindows[windowIndex] == NULL)
				{
					break;
				}
				else
				{
					if (pWindows[windowIndex]->zIndex == zIndex)
					{
						if (pWindows[windowIndex]->isVisible == GOS_TRUE)
						{
							//g_windowDraw(pWindows[windowIndex]);
							g_windowRefreshArea(pWindows[windowIndex], pWindow->xTop, pWindow->yTop, pWindow->width, pWindow->height);
						}
						break;
					}
				}
			}
			zIndex--;
		}

		break;
	}
	case EVENT_WINDOW_SHOWN:
	{
		// Draw top window (Z=0)
		zIndex = 0;

		// Search for topmost active window.
		while (zIndex < numOfWindows && breakLoop == GOS_FALSE)
		{
			for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
			{
				if (pWindows[windowIndex]->zIndex == zIndex &&
					pWindows[windowIndex]->isVisible == GOS_TRUE)
				{
					g_windowDraw(pWindows[windowIndex]);
					breakLoop = GOS_TRUE;
					break;
				}
			}
		}
		break;
	}
	}
}

/*GOS_STATIC void_t g_touchItCallback (void_t)
{
	ili9341_touch_interrupt(screen);
}*/

/*GOS_STATIC void_t g_screenTouchBegin(ili9341_t *lcd, u16_t x, u16_t y)
{
	u16_t zIndex = 0;
	u8_t windowIndex = 0u;
	bool_t breakLoop = GOS_FALSE;

	GOS_ISR_ENTER
	gos_traceTraceFormatted(GOS_TRUE, "Touch begin X: %d Y: %d\r\n", x, y);
	GOS_ISR_EXIT

	// Search for topmost active window.
	while (zIndex < numOfWindows && breakLoop == GOS_FALSE)
	{
		for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
		{
			if (pWindows[windowIndex]->zIndex == zIndex &&
				pWindows[windowIndex]->isVisible == GOS_TRUE)
			{
				if ((x >= pWindows[windowIndex]->xTop) && (x <= (pWindows[windowIndex]->xTop + pWindows[windowIndex]->width)) &&
					(y >= pWindows[windowIndex]->yTop) && (y <= (pWindows[windowIndex]->yTop + pWindows[windowIndex]->height)))
				{
					g_windowPressed(pWindows[windowIndex]);
				}

				breakLoop = GOS_TRUE;
				break;
			}
		}

		zIndex++;
	}
}*/

/*GOS_STATIC void_t g_screenTouchEnd(ili9341_t *lcd, u16_t x, u16_t y)
{
	u16_t zIndex = 0;
	u8_t windowIndex = 0u;
	bool_t breakLoop = GOS_FALSE;

	// Search for topmost active window.
	while (zIndex < numOfWindows && breakLoop == GOS_FALSE)
	{
		for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
		{
			if (pWindows[windowIndex]->zIndex == zIndex &&
				pWindows[windowIndex]->isVisible == GOS_TRUE)
			{
				if ((x >= pWindows[windowIndex]->xTop) && (x <= (pWindows[windowIndex]->xTop + pWindows[windowIndex]->width)) &&
					(y >= pWindows[windowIndex]->yTop) && (y <= (pWindows[windowIndex]->yTop + pWindows[windowIndex]->height)))
				{
					g_windowClicked(pWindows[windowIndex]);
				}

				breakLoop = GOS_TRUE;
				break;
			}
		}
		zIndex++;
	}
}*/

/*GOS_STATIC void_t g_windowPressed (g_window_t* pWindow)
{
	GOS_NOP;
}

GOS_STATIC void_t g_windowClicked (g_window_t* pWindow)
{

}*/
