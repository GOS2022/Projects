#include <g_button.h>
#include <g_window.h>
#include "string.h"

GOS_EXTERN gos_mutex_t g_mutex;

void_t g_buttonDraw (g_button_t* pButton)
{
	u16_t textIndex = 0u;
	u16_t btnWidth  = 0u;
	u16_t btnHeight = 0u;

	if (pButton->width == BUTTON_AUTO_WIDTH)
	{
		btnWidth = strlen(pButton->text) * ili9341_font_7x10.width + 6;
	}
	else
	{
		btnWidth = pButton->width;
	}

	if (pButton->height == BUTTON_AUTO_HEIGHT)
	{
		btnHeight = ili9341_font_7x10.height + 4;
	}
	else
	{
		btnHeight = pButton->height;
	}

	ili9341_fill_rect(pButton->backColor, pButton->xTop, pButton->yTop, btnWidth, btnHeight);
	ili9341_draw_rect(ILI9341_BLACK, pButton->xTop, pButton->yTop, btnWidth, btnHeight);

	if (pButton->isFocused == GOS_TRUE)
	{
		ili9341_draw_rect(ILI9341_YELLOW, pButton->xTop + 3, pButton->yTop + 3, btnWidth - 6, btnHeight - 6);
	}

	ili9341_text_attr_t attr =
	{
		.bg_color = pButton->backColor,
		.fg_color = pButton->foreColor,
		.font = &ili9341_font_7x10,
		.origin_x = pButton->xTop + ((btnWidth - strlen(pButton->text) * ili9341_font_7x10.width) / 2),
		.origin_y = pButton->yTop + ((btnHeight - ili9341_font_7x10.height) / 2)
	};

	while ((attr.origin_x + attr.font->width + 1) < (pButton->xTop + btnWidth) &&
			textIndex < BUTTON_TEXT_MAX_LENGTH &&
			pButton->text[textIndex] != '\0')
	{
		ili9341_draw_char(attr, pButton->text[textIndex++]);
		attr.origin_x += attr.font->width;
	}
}

void_t g_buttonDrawClick (g_button_t* pButton)
{
	u16_t textIndex = 0u;
	u16_t btnWidth  = 0u;
	u16_t btnHeight = 0u;

	if (pButton->width == BUTTON_AUTO_WIDTH)
	{
		btnWidth = strlen(pButton->text) * ili9341_font_7x10.width + 6;
	}
	else
	{
		btnWidth = pButton->width;
	}

	if (pButton->height == BUTTON_AUTO_HEIGHT)
	{
		btnHeight = ili9341_font_7x10.height + 4;
	}
	else
	{
		btnHeight = pButton->height;
	}

	ili9341_fill_rect(ILI9341_DARKGREY, pButton->xTop, pButton->yTop, btnWidth, btnHeight);
	ili9341_draw_rect(ILI9341_BLACK, pButton->xTop, pButton->yTop, btnWidth, btnHeight);

	ili9341_text_attr_t attr =
	{
		.bg_color = pButton->backColor,
		.fg_color = pButton->foreColor,
		.font = &ili9341_font_7x10,
		.origin_x = pButton->xTop + ((btnWidth - strlen(pButton->text) * ili9341_font_7x10.width) / 2),
		.origin_y = pButton->yTop + ((btnHeight - ili9341_font_7x10.height) / 2)
	};

	while ((attr.origin_x + attr.font->width + 1) < (pButton->xTop + btnWidth) &&
			textIndex < BUTTON_TEXT_MAX_LENGTH &&
			pButton->text[textIndex] != '\0')
	{
		ili9341_draw_char(attr, pButton->text[textIndex++]);
		attr.origin_x += attr.font->width;
	}
}

void_t g_buttonPress (g_button_t* pButton)
{
	if (pButton->isVisible == GOS_TRUE &&
		((g_window_t*)pButton->pOwner)->isVisible == GOS_TRUE &&
		pButton->isEnabled == GOS_TRUE)
	{
		g_buttonDrawClick(pButton);
		if (pButton->pressed != NULL)
		{
			pButton->pressed(pButton);
		}
	}
}

void_t g_buttonRelease (g_button_t* pButton)
{
	if (pButton->isVisible == GOS_TRUE &&
		((g_window_t*)pButton->pOwner)->isVisible == GOS_TRUE &&
		pButton->isEnabled == GOS_TRUE)
	{
		g_buttonDraw(pButton);
		if (pButton->released != NULL)
		{
			pButton->released(pButton);
		}
	}
}

void_t g_buttonFocus (g_button_t* pButton)
{
	if (pButton->isEnabled == GOS_TRUE &&
		pButton->isVisible == GOS_TRUE &&
		pButton->tabStop == GOS_TRUE &&
		pButton->isFocused != GOS_TRUE)
	{
		pButton->isFocused = GOS_TRUE;
		g_buttonDraw(pButton);
	}
}

void_t g_buttonDeFocus (g_button_t* pButton)
{
	if (pButton->isEnabled == GOS_TRUE &&
		pButton->isVisible == GOS_TRUE &&
		pButton->tabStop == GOS_TRUE &&
		pButton->isFocused == GOS_TRUE)
	{
		pButton->isFocused = GOS_FALSE;
		g_buttonDraw(pButton);
	}
}
