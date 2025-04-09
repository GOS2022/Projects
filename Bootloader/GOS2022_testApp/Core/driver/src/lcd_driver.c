/*
 * lcd_driver.c
 *
 *  Created on: Nov 11, 2022
 *      Author: Gabor
 */

#include "app.h"
#include "lcd_driver.h"
//#include "i2c_driver.h"
#include "drv_i2c.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define LCD_ADDRESS ( 0x44 )

#define LCD_DISPALY_CONTROL ( 0x0F )
#define LCD_FUNCTION_SET	( 0x38 )

// List of commands bit-fields.
// Display control
#define LCD_DISPLAY_CURSOR_BLINK (0x01U) //!< Blink cursor.
#define LCD_DISPLAY_CURSOR_ON    (0x02U) //!< Cursor ON.
#define LCD_DISPLAY_ON           (0x04U) //!< Display ON.
// Function set control
#define LCD_FUNCTION_N           (0x08U) //!< 4-bit mode.



// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit

u8_t 	_displaycontrol;
u8_t	actualRow;
u8_t	actualCol1, actualCol2;

GOS_EXTERN gos_tid_t lcdFirstLineHandler;
GOS_EXTERN gos_tid_t lcdSecondLineHandler;

GOS_STATIC gos_result_t lcd_driver_send_cmd (u8_t cmd);
GOS_STATIC gos_result_t lcd_driver_send_data (u8_t data);

GOS_STATIC void_t clear (void_t);


gos_result_t lcd_driver_init (void_t)
{
	gos_result_t lcdDriverInitResult = GOS_SUCCESS;

#if ORIGINAL_DRIVER
	gos_kernelDelayMs(20);
	lcd_driver_send_cmd(0x3);
	gos_kernelDelayMs(5);
	lcd_driver_send_cmd(0x3);
	gos_kernelDelayMs(1);
	lcd_driver_send_cmd(0x3);
	gos_kernelDelayMs(1);
	lcd_driver_send_cmd(0x2);
	gos_kernelDelayMs(1);


	// Enable 2 lines, data length 4 bits.
	lcd_driver_send_cmd(LCD_FUNCTIONSET | LCD_FUNCTION_N);
	// Display control: display ON, cursor OFF, cursor blinking OFF.
	lcd_driver_send_cmd((LCD_DISPLAYCONTROL | LCD_DISPLAY_ON) & ~LCD_DISPLAY_CURSOR_ON & ~LCD_DISPLAY_CURSOR_BLINK);
    // Clear LCD and return home.
	lcd_driver_send_cmd(LCD_CLEARDISPLAY);
	gos_kernelDelayMs(3);
#elif 0
	_addr = LCD_ADDRESS;
	_cols = 20;
	_rows = 4;
	_backlightval = LCD_BACKLIGHT;
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS | LCD_2LINE;

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	gos_kernelDelayMs(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	gos_kernelDelayMs(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	gos_kernelDelayUs(4500);

	// second try
	write4bits(0x03 << 4);
	gos_kernelDelayUs(4500);

	// third go!
	write4bits(0x03 << 4);
	gos_kernelDelayUs(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	home();
#else
	//gos_kernelDelayMs(20);
	gos_taskSleep(20);
	lcd_driver_send_cmd(0x3);
	//gos_kernelDelayMs(5);
	gos_taskSleep(5);
	lcd_driver_send_cmd(0x3);
	//gos_kernelDelayMs(1);
	gos_taskSleep(1);
	lcd_driver_send_cmd(0x3);
	//gos_kernelDelayMs(1);
	gos_taskSleep(1);
	lcd_driver_send_cmd(0x2);
	//gos_kernelDelayMs(1);
	gos_taskSleep(1);

	// set # lines, font size, etc.
	lcd_driver_send_cmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS | LCD_2LINE);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_driver_send_cmd(LCD_DISPLAYCONTROL | _displaycontrol);

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages).
	lcd_driver_send_cmd(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

	lcd_driver_send_cmd(LCD_RETURNHOME);  // set cursor position to zero
	//gos_kernelDelayUs(2000);
	// TODO
	gos_taskSleep(2);
#endif

	return lcdDriverInitResult;
}

GOS_STATIC bool_t writeStringReady = GOS_TRUE;

GOS_INLINE gos_result_t lcd_driver_write_string (u8_t line, const char_t* str)
{
	gos_result_t lcdDriverWriteStringResult = GOS_ERROR;
	u8_t index = 0u;

#if 0
	while (writeStringReady != GOS_TRUE)
	{
#if CFG_SCHED_COOPERATIVE
		gos_kernelTaskYield();
#else
		gos_taskSleep(5);
#endif
	}
	writeStringReady = GOS_FALSE;
#endif

	//if (gos_mutexLock(&lcdMutex, 500) == GOS_SUCCESS)
	{
		if (line < 2)
		{
			lcdDriverWriteStringResult = GOS_SUCCESS;

			lcd_driver_set_cursor(line, 0);

			for (index = 0u; index < 16u && str[index]; index++)
			{
				if (lcd_driver_send_data((u8_t)str[index]) != GOS_SUCCESS)
				{
					lcdDriverWriteStringResult = GOS_ERROR;
					break;
				}
			}
		}
	}
	//gos_mutexUnlock(&lcdMutex);

	writeStringReady = GOS_TRUE;

	return lcdDriverWriteStringResult;
}

GOS_STATIC bool_t setCursorReady = GOS_TRUE;

GOS_INLINE gos_result_t lcd_driver_set_cursor (u8_t row, u8_t col)
{
	gos_result_t lcdDriverSetCursorResult = GOS_SUCCESS;
	u8_t maskData = (col) & 0x0F;

#if 0
	while (setCursorReady != GOS_TRUE)
	{
#if CFG_SCHED_COOPERATIVE
		gos_kernelTaskYield();
#else
		gos_taskSleep(1);
#endif
	}

	setCursorReady = GOS_FALSE;
#endif

	switch (row)
	{
		case 0:
		{
			maskData |= 0x80;
			lcdDriverSetCursorResult = lcd_driver_send_cmd(maskData);
			break;
		}
		case 1:
		{
			maskData |= 0xC0;
			lcdDriverSetCursorResult = lcd_driver_send_cmd(maskData);
			break;
		}
		default:
		{
			break;
		}
	}

	//gos_taskSleep(1);

	actualRow = row;

	if (actualRow == 0)
	{
		actualCol1 = col;
	}
	else
	{
		actualCol2 = col;
	}

	setCursorReady = GOS_TRUE;

	return lcdDriverSetCursorResult;
}

gos_result_t lcd_driver_clear_display (void_t)
{
	gos_result_t lcdDriverClearDisplayResult = GOS_SUCCESS;

	clear();

	return lcdDriverClearDisplayResult;
}

GOS_STATIC gos_result_t lcd_driver_send_cmd (u8_t cmd)
{
	gos_result_t lcdDriverSendCmdResult = GOS_ERROR;
	u8_t data_u, data_l;
	u8_t data_t [4];

	data_u = ( cmd & 0xF0 );
	data_l = ( ( cmd << 4 ) & 0xF0 );
	data_t[0] = data_u | 0x0C;  //en=1, rs=0
	data_t[1] = data_u | 0x08;  //en=0, rs=0
	data_t[2] = data_l | 0x0C;  //en=1, rs=0
	data_t[3] = data_l | 0x08;  //en=0, rs=0

	//lcdDriverSendCmdResult = i2c_driver_transmit(LCD_ADDRESS, data_t, 4);
	lcdDriverSendCmdResult = drv_i2cTransmitIT(DRV_I2C_INSTANCE_1, LCD_ADDRESS, data_t, 4, GOS_MUTEX_ENDLESS_TMO, 100);

	//gos_taskSleep(1);

	return lcdDriverSendCmdResult;
}

GOS_STATIC gos_result_t lcd_driver_send_data (u8_t data)
{
	gos_result_t lcdDriverSendDataResult = GOS_ERROR;
	u8_t data_u, data_l;
	u8_t data_t [4];

	data_u = ( data & 0xF0 );
	data_l = ( ( data << 4 ) & 0xF0 );
	data_t[0] = data_u | 0x0D;  //en=1, rs=1
	data_t[1] = data_u | 0x09;  //en=0, rs=1
	data_t[2] = data_l | 0x0D;  //en=1, rs=1
	data_t[3] = data_l | 0x09;  //en=0, rs=1

	//lcdDriverSendDataResult = i2c_driver_transmit(LCD_ADDRESS, data_t, 4);
	//lcdDriverSendDataResult = drv_i2cTransmitBlocking(DRV_I2C_INSTANCE_1, LCD_ADDRESS, data_t, 4, GOS_MUTEX_ENDLESS_TMO, 1000);
	// TODO: check
	lcdDriverSendDataResult = drv_i2cTransmitIT(DRV_I2C_INSTANCE_1, LCD_ADDRESS, data_t, 4, GOS_MUTEX_ENDLESS_TMO, 100);

	gos_taskSleep(1);

	return lcdDriverSendDataResult;
}

/********** high level commands, for the user! */
GOS_STATIC void_t clear (void_t)
{
	lcd_driver_send_cmd(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	//gos_kernelDelayUs(2000);
	// TODO: check
	gos_taskSleep(2);
}

