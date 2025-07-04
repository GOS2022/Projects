/*
 * button.hpp
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */

#ifndef GOS_GRAPHICS_INC_G_BUTTON_H_
#define GOS_GRAPHICS_INC_G_BUTTON_H_

#include <g_control.h>
#include "gos.h"

#include "../../drv/inc/ili9341.h"
#include "../../drv/inc/ili9341_font.h"
#include "../../drv/inc/ili9341_gfx.h"

#define BUTTON_TEXT_MAX_LENGTH ( 24U )
#define BUTTON_AUTO_WIDTH      ( UINT16_MAX )
#define BUTTON_AUTO_HEIGHT     ( UINT16_MAX )


typedef struct g_button_t
{
	u16_t     xTop;
	u16_t     yTop;
	u16_t     width;
	u16_t     height;
	g_color_t foreColor;
	g_color_t backColor;
	bool_t    isEnabled;
	bool_t    isVisible;
	bool_t    isFocused;
	bool_t    tabStop;
	u16_t     tabIndex;
	char_t    text [BUTTON_TEXT_MAX_LENGTH];
	void_t*   pOwner;
	void_t    (*pressed)(struct g_button_t*);
	void_t    (*released)(struct g_button_t*);
}g_button_t;

void_t g_buttonDraw (g_button_t* pButton);

void_t g_buttonDrawClick (g_button_t* pButton);

void_t g_buttonPress (g_button_t* pButton);

void_t g_buttonRelease (g_button_t* pButton);

void_t g_buttonFocus (g_button_t* pButton);

void_t g_buttonDeFocus (g_button_t* pButton);

#endif /* GOS_GRAPHICS_INC_G_BUTTON_H_ */
