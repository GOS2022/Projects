/*
 * g_window.h
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */

#ifndef GOS_GRAPHICS_INC_G_WINDOW_H_
#define GOS_GRAPHICS_INC_G_WINDOW_H_

#include <g_control.h>
#include "gos.h"
#include <g_button.h>
#include <g_label.h>
#include "../../drv/inc/ili9341.h"
#include "../../drv/inc/ili9341_font.h"
#include "../../drv/inc/ili9341_gfx.h"

#define WINDOW_TITLE_MAX_LENGTH   ( 24u )
#define WINDOW_BUTTONS_MAX_NUMBER ( 5u  )
#define WINDOW_LABELS_MAX_NUMBER  ( 48u * 3u )
#define WINDOW_EVENT_MSG_ID       ( 0xAC20 )

__attribute__((unused)) typedef enum
{
	EVENT_STARTUP,
	EVENT_WINDOW_SHOWN,
	EVENT_WINDOW_HIDDEN
}g_windowEvent_t;

typedef struct g_window_t
{
	u16_t  xTop;
	u16_t  yTop;
	u16_t  width;
	u16_t  height;
	u16_t  zIndex;
	g_color_t backColor;
	bool_t isEnabled;
	bool_t isVisible;
	char_t title [WINDOW_TITLE_MAX_LENGTH];
	g_button_t* pButtons [WINDOW_BUTTONS_MAX_NUMBER];
	g_label_t* pLabels [WINDOW_LABELS_MAX_NUMBER];
	void_t (*windowPressed)(struct g_window_t*);
	void_t (*windowClicked)(struct g_window_t*);
}g_window_t;

void_t g_windowInit (g_window_t* pWindow, g_button_t* pCloseButton, g_label_t* pTitleLabel);

void_t g_windowDraw (g_window_t* pWindow);

void_t g_windowRefreshArea(g_window_t* pWindow, u16_t xTop, u16_t yTop, u16_t width, u16_t height);

void_t g_windowAddButton (g_window_t* pWindow, g_button_t* pButton);

void_t g_windowAddLabel (g_window_t* pWindow, g_label_t* pLabel);

gos_result_t g_windowShow (g_window_t* pWindow);

gos_result_t g_windowHide (g_window_t* pWindow);

gos_result_t g_windowTabStep (g_window_t* pWindow);

gos_result_t g_windowPressFocused (g_window_t* pWindow);

gos_result_t g_windowReleaseFocused (g_window_t* pWindow);

//void_t g_windowPressCloseButton (g_window_t* pWindow);

#endif /* GOS_GRAPHICS_INC_G_WINDOW_H_ */
