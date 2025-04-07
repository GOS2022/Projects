//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       g_driver.c
//! @author     Ahmed Gazar
//! @date       2025-04-07
//! @version    1.0
//!
//! @brief      GOS2022 Graphics Library / Driver Source
//! @details    For a more detailed description of this driver, please refer to @ref g_driver.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-04-07    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include "g_driver.h"
#include "ili9341.h"
#include "iodef.h"
#include <string.h>

/*
 * Static variables
 */
/**
 * Window pointer array.
 */
GOS_STATIC g_window_t* pWindows [G_DRIVER_WINDOW_MAX_NUM];

/**
 * Number of windows.
 */
GOS_STATIC u8_t numOfWindows;

#if 0
/**
 * Screen.
 */
GOS_STATIC ili9341_t* screen;
#endif

/*
 * Global variables
 */
/**
 * Graphics driver task ID.
 */
gos_tid_t   g_driverTaskId;

/**
 * Graphics driver mutex.
 */
gos_mutex_t g_mutex;

/*
 * Function prototypes
 */
GOS_STATIC void_t g_task (void_t);
GOS_STATIC void_t g_redrawHandler (g_windowEvent_t event, gos_message_t* pEventMessage);
#if 0
GOS_STATIC void_t g_touchItCallback (void_t);
GOS_STATIC void_t g_screenTouchBegin(ili9341_t *lcd, u16_t x, u16_t y);
GOS_STATIC void_t g_screenTouchEnd(ili9341_t *lcd, u16_t x, u16_t y);
GOS_STATIC void_t g_windowPressed (g_window_t* pWindow);
GOS_STATIC void_t g_windowClicked (g_window_t* pWindow);
#endif

/**
 * Graphics driver task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t g_taskDescriptor =
{
	.taskFunction       = g_task,
	.taskName           = "graphics_driver_task",
	.taskStackSize      = 0x600,
	.taskPriority       = 200,
	.taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: g_initGraphicsDriver
 */
gos_result_t g_initGraphicsDriver (/*ili9341_t* lcd*/)
{
#if 0
	gpio_callback_desc_t touchInterruptDesc =
	{
		.callback = g_touchItCallback,
		.pin = iodef_get(IO_TOUCH_INT)->pin
	};

	screen = lcd;
	drv_gpio_registerItCallback(&touchInterruptDesc);

	ili9341_set_touch_pressed_begin(g_screenTouchBegin);
	ili9341_set_touch_pressed_end(g_screenTouchEnd);
#endif
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	numOfWindows = 0u;
	GOS_CONCAT_RESULT(initResult, gos_mutexInit(&g_mutex));
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&g_taskDescriptor, &g_driverTaskId));

	return initResult;
}

/*
 * Function: g_registerWindow
 */
gos_result_t g_registerWindow (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;
	u16_t        windowIndex    = 0u;

	/*
	 * Function code.
	 */
	if (pWindow != NULL)
	{
		for (windowIndex = 0u; windowIndex < G_DRIVER_WINDOW_MAX_NUM; windowIndex++)
		{
			if (pWindows[windowIndex] == NULL)
			{
				pWindows[windowIndex] = pWindow;
				pWindow->zIndex = 0u;
				numOfWindows++;
				registerResult = GOS_SUCCESS;
				break;
			}
			else
			{
				pWindows[windowIndex]->zIndex++;
			}
		}
	}
	else
	{
		// Null pointer error.
	}

	return registerResult;
}

/*
 * Function: g_activateWindow
 */
gos_result_t g_activateWindow (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t activateResult = GOS_ERROR;
	u16_t        windowIndex    = 0u;
	u16_t        lastZIndex     = pWindow->zIndex;

	/*
	 * Function code.
	 */
	if (pWindow != NULL)
	{
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
		activateResult = GOS_SUCCESS;
	}
	else
	{
		// Null pointer error.
	}

	return activateResult;
}

/*
 * Function: g_inactivateWindow
 */
gos_result_t g_inactivateWindow (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t inactivateResult = GOS_ERROR;
	u16_t        windowIndex      = 0u;

	/*
	 * Function code.
	 */
	if (pWindow != NULL)
	{
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
		inactivateResult = GOS_SUCCESS;
	}
	else
	{
		// NULL pointer error.
	}

	return inactivateResult;
}

/*
 * Function: g_task
 */
GOS_STATIC void_t g_task (void_t)
{
	/*
	 * Local variables.
	 */
	gos_messageId_t selector [] = { WINDOW_EVENT_MSG_ID, 0 };
	gos_message_t   eventMessage;

	/*
	 * Function code.
	 */
	ili9341_fill_screen(ILI9341_BLACK);

	// Draw at startup.
	g_redrawHandler(EVENT_STARTUP, &eventMessage);

	for (;;)
	{
		if (gos_messageRx(selector, &eventMessage, GOS_MESSAGE_ENDLESS_TMO) == GOS_SUCCESS)
		{
			g_redrawHandler((g_windowEvent_t)eventMessage.messageBytes[0], &eventMessage);
		}
		else
		{
			(void_t) gos_taskSleep(50);
		}
	}
}

/*
 * Function: g_redrawHandler
 */
GOS_STATIC void_t g_redrawHandler (g_windowEvent_t event, gos_message_t* pEventMessage)
{
	/*
	 * Local variables.
	 */
	u16_t       windowIndex = 0u;
	u8_t        idx         = 0u;
	u8_t        zIndex      = 0u;
	bool_t      breakLoop   = GOS_FALSE;
	g_window_t* pWindow;

	/*
	 * Function code.
	 */
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
			(void_t) memcpy((void_t*)&pWindow, (void_t*)&pEventMessage->messageBytes[1], sizeof(pWindow));
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

#if 0
GOS_STATIC void_t g_touchItCallback (void_t)
{
	ili9341_touch_interrupt(screen);
}

GOS_STATIC void_t g_screenTouchBegin(ili9341_t *lcd, u16_t x, u16_t y)
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
}

GOS_STATIC void_t g_screenTouchEnd(ili9341_t *lcd, u16_t x, u16_t y)
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
}

GOS_STATIC void_t g_windowPressed (g_window_t* pWindow)
{
	GOS_NOP;
}

GOS_STATIC void_t g_windowClicked (g_window_t* pWindow)
{

}
#endif
