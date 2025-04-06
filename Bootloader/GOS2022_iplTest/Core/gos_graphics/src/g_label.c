/*
 * g_label.c
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */
#include <g_label.h>
#include "string.h"

#define LABEL_DRAW_MUTEX_TMO_MS ( 3000u )

GOS_EXTERN gos_mutex_t g_mutex;

void_t g_labelDraw (g_label_t* pLabel)
{
	//if (gos_mutexLock(&g_mutex, LABEL_DRAW_MUTEX_TMO_MS) == GOS_SUCCESS)
	{
		u16_t textIndex = 0u;
		u16_t lblWidth  = 0u;
		u16_t lblHeight = 0u;

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
	//(void_t) gos_mutexUnlock(&g_mutex);
}

void_t g_labelSetText (g_label_t* pLabel, char_t* text)
{
	if (pLabel != NULL && text != NULL && pLabel->text != NULL)
	{
		if (strcmp(text, pLabel->text) != 0)
		{
			if (strlen(text) < LABEL_TEXT_MAX_LENGTH)
			{
				strcpy(pLabel->text, text);
#include "g_window.h"
				if (((g_window_t*)(pLabel->pOwner))->zIndex == 0u)
				{
					if (gos_mutexLock(&g_mutex, LABEL_DRAW_MUTEX_TMO_MS) == GOS_SUCCESS)
					{
						g_labelDraw(pLabel);
					}
					(void_t) gos_mutexUnlock(&g_mutex);
				}
			}
		}
	}
}
