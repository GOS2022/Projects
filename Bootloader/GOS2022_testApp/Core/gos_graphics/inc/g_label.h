/*
 * g_label.h
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */

#ifndef GOS_GRAPHICS_INC_G_LABEL_H_
#define GOS_GRAPHICS_INC_G_LABEL_H_

#include <g_control.h>
#include "gos.h"
#include "ili9341.h"
#include "ili9341_gfx.h"
#include "ili9341_font.h"

#define LABEL_TEXT_MAX_LENGTH ( 60U )
#define LABEL_AUTO_WIDTH      ( UINT16_MAX )
#define LABEL_AUTO_HEIGHT     ( UINT16_MAX )

typedef struct
{
	u16_t  xTop;
	u16_t  yTop;
	u16_t  width;
	u16_t  height;
	g_color_t foreColor;
	g_color_t backColor;
	bool_t isEnabled;
	bool_t isVisible;
	char_t text [LABEL_TEXT_MAX_LENGTH];
	void_t* pOwner;
}g_label_t;

void_t g_labelDraw (g_label_t* pLabel);

gos_result_t g_labelSetText (g_label_t* pLabel, char_t* text);

#endif /* GOS_GRAPHICS_INC_G_LABEL_H_ */
