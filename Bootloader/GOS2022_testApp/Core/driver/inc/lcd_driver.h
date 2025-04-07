/*
 * lcd_driver.h
 *
 *  Created on: Nov 11, 2022
 *      Author: Gabor
 */

#ifndef DRIVER_INC_LCD_DRIVER_H_
#define DRIVER_INC_LCD_DRIVER_H_

#include "gos.h"

/*#define LCD_MAX_LINE_LENGTH	( 128 )

typedef enum
{
	LCD_DISPLAY_NORMAL = 0u,
	LCD_DISPLAY_SHIFT_RIGHT,
	LCD_DISPLAY_SHIFT_LEFT,
	LCD_DISPLAY_CYCLIC_RIGHT,
	LCD_DISPLAY_CYCLIC_LEFT,
	LCD_DISPLAY_BLINK
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
}lcd_display_cfg_t;*/

gos_result_t lcd_driver_init (void_t);

gos_result_t lcd_driver_write_next_string (u8_t line, char_t* str);

//gos_result_t lcd_driver_display_text (lcd_display_cfg_t* config, const char_t* str, ...);

gos_result_t lcd_driver_write_string (u8_t line, const char_t* str/*, ...*/);

gos_result_t lcd_driver_set_cursor (u8_t row, u8_t col);

gos_result_t lcd_driver_select_first_line (void_t);

gos_result_t lcd_driver_select_second_line (void_t);

gos_result_t lcd_driver_clear_display (void_t);

//void_t lcd_driver_task (void_t);

void_t lcd_driver_first_line_handler (void_t);

void_t lcd_driver_second_line_handler (void_t);

#endif /* DRIVER_INC_LCD_DRIVER_H_ */
