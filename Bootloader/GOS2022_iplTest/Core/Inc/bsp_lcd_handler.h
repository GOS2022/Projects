/*
 * bsp_lcd_handler.h
 *
 *  Created on: Oct 30, 2023
 *      Author: Gabor
 */

#ifndef INC_BSP_LCD_HANDLER_H_
#define INC_BSP_LCD_HANDLER_H_

#include "gos.h"

typedef enum
{
	LCD_DISPLAY_NORMAL = 0u,
	LCD_DISPLAY_SHIFT_RIGHT,
	LCD_DISPLAY_SHIFT_LEFT,
	LCD_DISPLAY_CYCLIC_RIGHT,
	LCD_DISPLAY_CYCLIC_LEFT,
	LCD_DISPLAY_BLINK,
	LCD_DISPLAY_SHIFT_WAIT_1500,
	LCD_DISPLAY_SHIFT_WAIT_500
}lcd_display_mode_t;

typedef enum
{
	LCD_BLINK_STATE_ON = 0u,
	LCD_BLINK_STATE_OFF = 1u
}lcd_display_blink_state_t;

typedef struct
{
	u8_t line;
	lcd_display_mode_t displayMode;
	lcd_display_mode_t originalDisplayMode;
	u32_t periodMs;
	u32_t lastTick;
	u32_t tickStart;
	u8_t blinkStartIndex;
	u8_t blinkEndIndex;
	u8_t shiftCounter;
	lcd_display_blink_state_t blinkState;
}lcd_display_cfg_t;

gos_result_t BSP_LCD_HandlerInit (void_t);

gos_result_t BSP_LCD_HandlerDisplayText (lcd_display_cfg_t* config,  const char_t* text, ...);

gos_result_t BSP_LCD_HandlerWriteNextString (u8_t line, char_t* str);

#endif /* INC_BSP_LCD_HANDLER_H_ */
