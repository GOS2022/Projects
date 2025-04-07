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
//! @file       g_window.c
//! @author     Ahmed Gazar
//! @date       2025-04-07
//! @version    1.0
//!
//! @brief      GOS2022 Graphics Library / Window Control Source
//! @details    For a more detailed description of this driver, please refer to @ref g_window.h
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
#include <g_window.h>
#include <g_driver.h>
#include "string.h"

/*
 * Macros
 */
/**
 * Window close button width.
 */
#define CLOSE_BUTTON_WIDTH  ( 20u )

/**
 * Window close button height.
 */
#define CLOSE_BUTTON_HEIGHT ( 20u )

/**
 * Mutex timeout [ms].
 */
#define G_MUTEX_TMO_MS      ( 5000u )

/*
 * Static variabless
 */
/**
 * Window shown message.
 */
GOS_STATIC gos_message_t windowShownMessage =
{
	.messageId = WINDOW_EVENT_MSG_ID,
	.messageBytes[0] = EVENT_WINDOW_SHOWN,
	.messageSize = 1
};

/**
 * Window hidden message.
 */
GOS_STATIC gos_message_t windowHiddenMessage =
{
	.messageId = WINDOW_EVENT_MSG_ID,
	.messageBytes[0] = EVENT_WINDOW_HIDDEN,
	.messageSize = 1
};

/*
 * External variables
 */
GOS_EXTERN gos_tid_t   g_driverTaskId;
GOS_EXTERN gos_mutex_t g_mutex;

/*
 * Function prototypes
 */
GOS_STATIC void_t g_windowCloseButtonPressedCallback  (g_button_t* pButton);
GOS_STATIC void_t g_windowCloseButtonReleasedCallback (g_button_t* pButton);

/*
 * Function: g_windowInit
 */
void_t g_windowInit (g_window_t* pWindow, g_button_t* pCloseButton, g_label_t* pTitleLabel)
{
	/*
	 * Function code.
	 */
	pCloseButton->backColor = ILI9341_RED;
	pCloseButton->foreColor = ILI9341_WHITE;
	pCloseButton->xTop = pWindow->xTop + pWindow->width - CLOSE_BUTTON_WIDTH;
	pCloseButton->yTop = pWindow->yTop;
	pCloseButton->height = CLOSE_BUTTON_HEIGHT;
	pCloseButton->width  = CLOSE_BUTTON_WIDTH;
	pCloseButton->pressed = g_windowCloseButtonPressedCallback;
	pCloseButton->released = g_windowCloseButtonReleasedCallback;
	pCloseButton->tabIndex = 255;
	pCloseButton->tabStop = GOS_TRUE;
	pCloseButton->isEnabled = GOS_TRUE;
	pCloseButton->isFocused = GOS_FALSE;
	pCloseButton->isVisible = GOS_TRUE;
	(void_t) strcpy(pCloseButton->text, "X");

	g_windowAddButton(pWindow, pCloseButton);

	pTitleLabel->foreColor = ILI9341_WHITE;
	pTitleLabel->backColor = ILI9341_BLUE;
	pTitleLabel->xTop = pWindow->xTop + 3;
	pTitleLabel->yTop = pWindow->yTop + 3;
	pTitleLabel->height = LABEL_AUTO_HEIGHT;
	pTitleLabel->width  = LABEL_AUTO_WIDTH;
	(void_t) strcpy(pTitleLabel->text, pWindow->title);
	g_windowAddLabel(pWindow, pTitleLabel);
}

/*
 * Function: g_windowDraw
 */
void_t g_windowDraw (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	u8_t controlIndex = 0u;

	/*
	 * Function code.
	 */
	if ((pWindow != NULL) && (gos_mutexLock(&g_mutex, G_MUTEX_TMO_MS) == GOS_SUCCESS))
	{
		// Draw header line.
		ili9341_fill_rect(ILI9341_BLUE, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);
		ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);

		// Draw window frame.
		ili9341_fill_rect(pWindow->backColor, pWindow->xTop, pWindow->yTop + CLOSE_BUTTON_HEIGHT, pWindow->width, pWindow->height);
		ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop + CLOSE_BUTTON_HEIGHT, pWindow->width, pWindow->height);

		// Draw buttons.
		for (controlIndex = 0u; controlIndex < WINDOW_BUTTONS_MAX_NUMBER; controlIndex++)
		{
			if (pWindow->pButtons[controlIndex] != NULL)
			{
				g_buttonDraw(pWindow->pButtons[controlIndex]);
			}
			else
			{
				break;
			}
		}

		// Draw labels.
		for (controlIndex = 0u; controlIndex < WINDOW_LABELS_MAX_NUMBER; controlIndex++)
		{
			if (pWindow->pLabels[controlIndex] != NULL)
			{
				g_labelDraw(pWindow->pLabels[controlIndex]);
			}
			else
			{
				break;
			}
		}

		(void_t) gos_mutexUnlock(&g_mutex);
	}
	else
	{
		// NULL pointer or mutex error.
	}
}

/*
 * Function: g_windowRefreshArea
 */
void_t g_windowRefreshArea (g_window_t* pWindow, u16_t xTop, u16_t yTop, u16_t width, u16_t height)
{
	/*
	 * Local variables.
	 */
	u8_t controlIndex = 0u;

	/*
	 * Function code.
	 */
	if ((pWindow != NULL) && (gos_mutexLock(&g_mutex, G_MUTEX_TMO_MS) == GOS_SUCCESS))
	{
		if (yTop < CLOSE_BUTTON_HEIGHT)
		{
			// Draw header line.
			ili9341_fill_rect(ILI9341_BLUE, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);
			ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);

			// Draw window frame.
			ili9341_fill_rect(pWindow->backColor, xTop, CLOSE_BUTTON_HEIGHT, width, height - (CLOSE_BUTTON_HEIGHT - yTop));
			ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop + CLOSE_BUTTON_HEIGHT, pWindow->width, pWindow->height);
		}
		else
		{
			// Redraw background.
			ili9341_fill_rect(pWindow->backColor, xTop, yTop, width, height + CLOSE_BUTTON_HEIGHT);
		}

		// Draw buttons.
		for (controlIndex = 0u; controlIndex < WINDOW_BUTTONS_MAX_NUMBER; controlIndex++)
		{
			if (pWindow->pButtons[controlIndex] != NULL &&
			   ((pWindow->pButtons[controlIndex]->xTop < (xTop + width)) ||
		       ((pWindow->pButtons[controlIndex]->xTop + width) > xTop)
			   ))
			{
				// TODO y-axis check is missing.
				g_buttonDraw(pWindow->pButtons[controlIndex]);
			}
			else
			{
				break;
			}
		}

		// Draw labels.
		for (controlIndex = 0u; controlIndex < WINDOW_LABELS_MAX_NUMBER; controlIndex++)
		{
			if (pWindow->pLabels[controlIndex] != NULL &&
		       ((pWindow->pLabels[controlIndex]->xTop < (xTop + width)) ||
			   ((pWindow->pLabels[controlIndex]->xTop + width) > xTop)
			   ))
			{
				g_labelDraw(pWindow->pLabels[controlIndex]);
			}
			else
			{
				break;
			}
		}

		(void_t) gos_mutexUnlock(&g_mutex);
	}
	else
	{
		// NULL pointer or mutex error.
	}
}

/*
 * Function: g_windowAddButton
 */
void_t g_windowAddButton (g_window_t* pWindow, g_button_t* pButton)
{
	/*
	 * Local variables.
	 */
	u8_t buttonIndex = 0u;
	// TODO check if button is within window

	/*
	 * Function code.
	 */
	for (buttonIndex = 0u; buttonIndex < WINDOW_BUTTONS_MAX_NUMBER; buttonIndex++)
	{
		if (pWindow->pButtons[buttonIndex] == NULL)
		{
			pWindow->pButtons[buttonIndex] = pButton;
			pButton->pOwner = (void_t*)pWindow;
			return;
		}
	}
}

/*
 * Function: g_windowAddLabel
 */
void_t g_windowAddLabel (g_window_t* pWindow, g_label_t* pLabel)
{
	/*
	 * Local variables.
	 */
	u8_t labelIndex = 0u;
	// TODO check if label is within window

	/*
	 * Function code.
	 */
	for (labelIndex = 0u; labelIndex < WINDOW_LABELS_MAX_NUMBER; labelIndex++)
	{
		if (pWindow->pLabels[labelIndex] == NULL)
		{
			pWindow->pLabels[labelIndex] = pLabel;
			pLabel->pOwner = (void_t*)pWindow;
			return;
		}
	}
}

/*
 * Function: g_windowShow
 */
gos_result_t g_windowShow (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t showResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	if (pWindow != NULL)
	{
		if (pWindow->isVisible == GOS_FALSE)
		{
			pWindow->isVisible = GOS_TRUE;
			GOS_CONCAT_RESULT(showResult, g_activateWindow(pWindow));
			GOS_CONCAT_RESULT(showResult, gos_messageTx(&windowShownMessage));
		}
		else
		{
			// Already visible. Nothing to do.
		}
	}
	else
	{
		// NULL pointer.
		showResult = GOS_ERROR;
	}

	return showResult;
}

/*
 * Function: g_windowHide
 */
gos_result_t g_windowHide (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t hideResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	if (pWindow != NULL)
	{
		if (pWindow->isVisible == GOS_TRUE)
		{
			pWindow->isVisible = GOS_FALSE;
			GOS_CONCAT_RESULT(hideResult, g_inactivateWindow(pWindow));
			(void_t) memcpy((void_t*)&windowHiddenMessage.messageBytes[1], (void_t*)&pWindow, sizeof(pWindow));
			windowHiddenMessage.messageSize = 1 + sizeof(pWindow);
			GOS_CONCAT_RESULT(hideResult, gos_messageTx(&windowHiddenMessage));
		}
		else
		{
			// Already hidden. Nothing to do.
		}
	}
	else
	{
		// NULL pointer.
		hideResult = GOS_ERROR;
	}

	return hideResult;
}

/*
 * Function: g_windowTabStep
 */
gos_result_t g_windowTabStep (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t tabStepResult = GOS_ERROR;
	u8_t         focusedTabIdx = 0u;
	u8_t         focusedIdx    = 0u;
	u8_t         nextTabIdx    = 0u;
	u8_t         idx           = 0u;

	/*
	 * Function code.
	 */
	if ((pWindow != NULL) && (pWindow->isEnabled == GOS_TRUE) && (pWindow->isVisible == GOS_TRUE))
	{
		for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
		{
			if (pWindow->pButtons[idx]->isFocused == GOS_TRUE)
			{
				focusedTabIdx = pWindow->pButtons[idx]->tabIndex;
				focusedIdx = idx;
				break;
			}
			else
			{
				// Continue.
			}
		}

		for (nextTabIdx = focusedTabIdx + 1; nextTabIdx <= 255; nextTabIdx++)
		{
			for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
			{
				if ((pWindow->pButtons[idx]->tabStop  == GOS_TRUE) &&
					(pWindow->pButtons[idx]->tabIndex == nextTabIdx))
				{
					g_buttonDeFocus(pWindow->pButtons[focusedIdx]);
					g_buttonFocus(pWindow->pButtons[idx]);
					return GOS_SUCCESS;
				}
			}
		}

		for (nextTabIdx = 0u; nextTabIdx < focusedTabIdx; idx++)
		{
			for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
			{
				if ((pWindow->pButtons[idx]->tabStop  == GOS_TRUE) &&
					(pWindow->pButtons[idx]->tabIndex == nextTabIdx))
				{
					g_buttonDeFocus(pWindow->pButtons[focusedTabIdx]);
					g_buttonFocus(pWindow->pButtons[idx]);
					return GOS_SUCCESS;
				}
			}
		}
	}
	else
	{
		// NULL pointer, or disabled, or hidden.
	}

	return tabStepResult;
}

/*
 * Function: g_windowPressFocused
 */
gos_result_t g_windowPressFocused (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t pressFocusedResult = GOS_ERROR;
	u8_t         idx                = 0u;

	/*
	 * Function code.
	 */
	if ((pWindow != NULL) && (pWindow->isEnabled == GOS_TRUE) && (pWindow->isVisible == GOS_TRUE))
	{
		for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
		{
			if (pWindow->pButtons[idx]->isFocused == GOS_TRUE)
			{
				g_buttonDrawClick(pWindow->pButtons[idx]);
				// Check if callback exists, and if so, call it.
				if (pWindow->pButtons[idx]->pressed != NULL)
				{
					pWindow->pButtons[idx]->pressed(pWindow->pButtons[idx]);
				}
				pressFocusedResult = GOS_SUCCESS;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// NULL pointer, or disabled, or hidden.
	}

	return pressFocusedResult;
}

/*
 * Function: g_windowReleaseFocused
 */
gos_result_t g_windowReleaseFocused (g_window_t* pWindow)
{
	/*
	 * Local variables.
	 */
	gos_result_t releaseFocusedResult = GOS_ERROR;
	u8_t         idx                  = 0u;

	/*
	 * Function code.
	 */
	if ((pWindow != NULL) && (pWindow->isEnabled == GOS_TRUE) && (pWindow->isVisible == GOS_TRUE))
	{
		for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
		{
			if (pWindow->pButtons[idx]->isFocused == GOS_TRUE)
			{
				g_buttonDraw(pWindow->pButtons[idx]);

				if (pWindow->pButtons[idx]->released != NULL)
				{
					pWindow->pButtons[idx]->released(pWindow->pButtons[idx]);
				}
				releaseFocusedResult = GOS_SUCCESS;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// NULL pointer, or disabled, or hidden.
	}

	return releaseFocusedResult;
}

/**
 * TODO
 * @param pButton
 * @return
 */
GOS_STATIC void_t g_windowCloseButtonPressedCallback (g_button_t* pButton)
{
	//g_buttonDrawClick(pButton);
}

/**
 * TODO
 * @param pButton
 * @return
 */
GOS_STATIC void_t g_windowCloseButtonReleasedCallback (g_button_t* pButton)
{
	//g_buttonDraw(pButton);
	g_windowHide((g_window_t*)pButton->pOwner);
}
