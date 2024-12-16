/*
 * g_window.c
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */

#include <g_window.h>
#include <g_driver.h>
#include "string.h"

#define CLOSE_BUTTON_WIDTH  ( 20U )
#define CLOSE_BUTTON_HEIGHT ( 20U )

#define G_MUTEX_TMO_MS      ( 5000u )

GOS_EXTERN gos_tid_t g_driverTaskId;
GOS_EXTERN gos_mutex_t g_mutex;

GOS_STATIC gos_message_t windowShownMessage =
{
	.messageId = WINDOW_EVENT_MSG_ID,
	.messageBytes[0] = EVENT_WINDOW_SHOWN,
	.messageSize = 1
};

GOS_STATIC gos_message_t windowHiddenMessage =
{
	.messageId = WINDOW_EVENT_MSG_ID,
	.messageBytes[0] = EVENT_WINDOW_HIDDEN,
	.messageSize = 1
};

GOS_STATIC void_t g_windowCloseButtonPressedCallback (g_button_t* pButton);
GOS_STATIC void_t g_windowCloseButtonReleasedCallback (g_button_t* pButton);

void_t g_windowInit (g_window_t* pWindow, g_button_t* pCloseButton, g_label_t* pTitleLabel)
{
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
	strcpy(pCloseButton->text, "X");
	g_windowAddButton(pWindow, pCloseButton);

	pTitleLabel->foreColor = ILI9341_WHITE;
	pTitleLabel->backColor = ILI9341_BLUE;
	pTitleLabel->xTop = pWindow->xTop + 3;
	pTitleLabel->yTop = pWindow->yTop + 3;
	pTitleLabel->height = LABEL_AUTO_HEIGHT;
	pTitleLabel->width  = LABEL_AUTO_WIDTH;
	strcpy(pTitleLabel->text, pWindow->title);
	g_windowAddLabel(pWindow, pTitleLabel);
}

void_t g_windowDraw (g_window_t* pWindow)
{
	if (gos_mutexLock(&g_mutex, G_MUTEX_TMO_MS) == GOS_SUCCESS)
	{
		//u16_t textIndex = 0u;
		u8_t controlIndex = 0u;

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
}

void_t g_windowRefreshArea(g_window_t* pWindow, u16_t xTop, u16_t yTop, u16_t width, u16_t height)
{
	if (gos_mutexLock(&g_mutex, G_MUTEX_TMO_MS) == GOS_SUCCESS)
	{
		//u16_t textIndex = 0u;
		u8_t controlIndex = 0u;

		if (yTop < CLOSE_BUTTON_HEIGHT)
		{
			// Draw header line.
			ili9341_fill_rect(ILI9341_BLUE, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);
			ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop, pWindow->width, CLOSE_BUTTON_HEIGHT);

			// Draw window frame.
			ili9341_fill_rect(pWindow->backColor, xTop, CLOSE_BUTTON_HEIGHT, width, height - (CLOSE_BUTTON_HEIGHT - yTop));
			ili9341_draw_rect(ILI9341_BLACK, pWindow->xTop, pWindow->yTop + CLOSE_BUTTON_HEIGHT, pWindow->width, pWindow->height);

			/*closeButton.backColor = ILI9341_RED;
			closeButton.foreColor = ILI9341_WHITE;
			closeButton.xTop = pWindow->xTop + pWindow->width - closeButton.width;
			closeButton.yTop = pWindow->yTop;
			g_buttonDraw(&closeButton);*/

			/*ili9341_text_attr_t attr =
			{
				.font = &ili9341_font_7x10,
				.origin_x = pWindow->xTop + 3,
				.origin_y = pWindow->yTop + ((CLOSE_BUTTON_HEIGHT - ili9341_font_7x10.height) / 2)
			};

			attr.bg_color = ILI9341_BLUE;
			attr.fg_color = ILI9341_WHITE;

			while ((attr.origin_x + attr.font->width + 1) < (pWindow->xTop + pWindow->width - CLOSE_BUTTON_WIDTH) &&
					textIndex < WINDOW_TITLE_MAX_LENGTH &&
					pWindow->title[textIndex] != '\0')
			{
				ili9341_draw_char(attr, pWindow->title[textIndex++]);
				attr.origin_x += attr.font->width;
			}*/
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
}

void_t g_windowAddButton (g_window_t* pWindow, g_button_t* pButton)
{
	u8_t buttonIndex = 0u;
	// TODO check if button is within window

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

void_t g_windowAddLabel (g_window_t* pWindow, g_label_t* pLabel)
{
	u8_t labelIndex = 0u;
	// TODO check if button is within window

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

void_t g_windowShow (g_window_t* pWindow)
{
	if (pWindow->isVisible == GOS_FALSE)
	{
		pWindow->isVisible = GOS_TRUE;
		g_activateWindow(pWindow);
		gos_messageTx(&windowShownMessage);
	}
}

void_t g_windowHide (g_window_t* pWindow)
{
	if (pWindow->isVisible == GOS_TRUE)
	{
		pWindow->isVisible = GOS_FALSE;
		g_inactivateWindow(pWindow);
		(void_t) memcpy((void_t*)&windowHiddenMessage.messageBytes[1], (void_t*)&pWindow, sizeof(pWindow));
		windowHiddenMessage.messageSize = 1 + sizeof(pWindow);
		gos_messageTx(&windowHiddenMessage);
	}
}

GOS_STATIC void_t g_windowCloseButtonPressedCallback (g_button_t* pButton)
{
	//g_buttonDrawClick(pButton);
}

GOS_STATIC void_t g_windowCloseButtonReleasedCallback (g_button_t* pButton)
{
	//g_buttonDraw(pButton);
	g_windowHide((g_window_t*)pButton->pOwner);
}

void_t g_windowTabStep (g_window_t* pWindow)
{
	u8_t focusedTabIdx = 0;
	u8_t focusedIdx = 0;
	u8_t nextTabIdx = 0;
	u8_t idx = 0u;

	if (pWindow->isEnabled == GOS_TRUE && pWindow->isVisible == GOS_TRUE)
	{
		for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
		{
			if (pWindow->pButtons[idx]->isFocused == GOS_TRUE)
			{
				focusedTabIdx = pWindow->pButtons[idx]->tabIndex;
				focusedIdx = idx;
				break;
			}
		}

		for (nextTabIdx = focusedTabIdx + 1; nextTabIdx <= 255; nextTabIdx++)
		{
			for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
			{
				if (pWindow->pButtons[idx]->tabStop == GOS_TRUE &&
					pWindow->pButtons[idx]->tabIndex == nextTabIdx)
				{
					g_buttonDeFocus(pWindow->pButtons[focusedIdx]);
					g_buttonFocus(pWindow->pButtons[idx]);
					return;
				}
			}
		}

		for (nextTabIdx = 0u; nextTabIdx < focusedTabIdx; idx++)
		{
			for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
			{
				if (pWindow->pButtons[idx]->tabStop == GOS_TRUE &&
					pWindow->pButtons[idx]->tabIndex == nextTabIdx)
				{
					g_buttonDeFocus(pWindow->pButtons[focusedTabIdx]);
					g_buttonFocus(pWindow->pButtons[idx]);
					return;
				}
			}
		}
	}
}

void_t g_windowPressFocused (g_window_t* pWindow)
{
	u8_t idx = 0u;

	if (pWindow->isEnabled == GOS_TRUE && pWindow->isVisible == GOS_TRUE)
	{
		for (idx = 0u; idx < WINDOW_BUTTONS_MAX_NUMBER; idx++)
		{
			if (pWindow->pButtons[idx]->isFocused == GOS_TRUE)
			{
				g_buttonDrawClick(pWindow->pButtons[idx]);
				if (pWindow->pButtons[idx]->pressed != NULL)
				{
					pWindow->pButtons[idx]->pressed(pWindow->pButtons[idx]);
				}
				return;
			}
		}
	}
}

void_t g_windowReleaseFocused (g_window_t* pWindow)
{
	u8_t idx = 0u;

	if (pWindow->isEnabled == GOS_TRUE && pWindow->isVisible == GOS_TRUE)
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
				return;
			}
		}
	}
}
