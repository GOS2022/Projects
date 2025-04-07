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
//! @file       g_label.c
//! @author     Ahmed Gazar
//! @date       2025-04-07
//! @version    1.0
//!
//! @brief      GOS2022 Graphics Library / Label Control Source
//! @details    For a more detailed description of this driver, please refer to @ref g_label.h
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
#include <g_label.h>
#include "string.h"

/*
 * Macros
 */
/**
 * Mutex timeout for label drawing.
 */
#define LABEL_DRAW_MUTEX_TMO_MS ( 3000u )

/*
 * External variables
 */
GOS_EXTERN gos_mutex_t g_mutex;

/*
 * Function: g_labelDraw
 */
void_t g_labelDraw (g_label_t* pLabel)
{
	/*
	 * Local variables.
	 */
	u16_t textIndex = 0u;
	u16_t lblWidth  = 0u;
	u16_t lblHeight = 0u;

	/*
	 * Function code.
	 */
	if (pLabel->width == LABEL_AUTO_WIDTH)
	{
		lblWidth = strlen(pLabel->text) * ili9341_font_7x10.width + 6;
	}
	else
	{
		lblWidth = pLabel->width;
	}

	if (pLabel->height == LABEL_AUTO_HEIGHT)
	{
		lblHeight = ili9341_font_7x10.height + 4;
	}
	else
	{
		lblHeight = pLabel->height;
	}

	ili9341_text_attr_t attr =
	{
		.bg_color = pLabel->backColor,
		.fg_color = pLabel->foreColor,
		.font = &ili9341_font_7x10,
		.origin_x = pLabel->xTop + 3,
		.origin_y = pLabel->yTop + ((lblHeight - ili9341_font_7x10.height) / 2)
	};

	while ((attr.origin_x + attr.font->width + 1) < (pLabel->xTop + lblWidth) &&
			textIndex < LABEL_TEXT_MAX_LENGTH &&
			pLabel->text[textIndex] != '\0')
	{
		ili9341_draw_char(attr, pLabel->text[textIndex++]);
		attr.origin_x += attr.font->width;
	}
}

/*
 * Function: g_labelSetText
 */
gos_result_t g_labelSetText (g_label_t* pLabel, char_t* text)
{
	/*
	 * Local variables
	 */
	gos_result_t setResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pLabel != NULL) && (text != NULL) && (pLabel->text != NULL))
	{
		if (strcmp(text, pLabel->text) != 0)
		{
			if (strlen(text) < LABEL_TEXT_MAX_LENGTH)
			{
				// Set text.
				(void_t) strcpy(pLabel->text, text);
#include "g_window.h"
				if (((g_window_t*)(pLabel->pOwner))->zIndex == 0u)
				{
					if (gos_mutexLock(&g_mutex, LABEL_DRAW_MUTEX_TMO_MS) == GOS_SUCCESS)
					{
						g_labelDraw(pLabel);
						setResult = GOS_SUCCESS;
					}
					else
					{
						// Mutex error.
						setResult = GOS_ERROR;
					}
					(void_t) gos_mutexUnlock(&g_mutex);
				}
				else
				{
					// Text copied, drawing not needed (not in focus).
					setResult = GOS_SUCCESS;
				}
			}
			else
			{
				// Text too long.
			}
		}
		else
		{
			// Text is the same. No need to update.
			setResult = GOS_SUCCESS;
		}
	}
	else
	{
		// NULL pointer error.
	}

	return setResult;
}
