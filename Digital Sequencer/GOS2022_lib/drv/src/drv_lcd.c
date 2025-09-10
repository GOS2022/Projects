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
//! @file       drv_lcd.c
//! @author     Ahmed Gazar
//! @date       2025-04-21
//! @version    1.0
//!
//! @brief      GOS2022 Library / 16x2 LCD display driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_lcd.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-04-21    Ahmed Gazar     Initial version created.
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
#include <drv_lcd.h>

/*
 * Macros
 */
// commands
#define LCD_CLEARDISPLAY        ( 0x01 )
#define LCD_RETURNHOME          ( 0x02 )
#define LCD_ENTRYMODESET        ( 0x04 )
#define LCD_DISPLAYCONTROL      ( 0x08 )
#define LCD_CURSORSHIFT         ( 0x10 )
#define LCD_FUNCTIONSET         ( 0x20 )
#define LCD_SETCGRAMADDR        ( 0x40 )
#define LCD_SETDDRAMADDR        ( 0x80 )

// flags for display entry mode
#define LCD_ENTRYRIGHT          ( 0x00 )
#define LCD_ENTRYLEFT           ( 0x02 )
#define LCD_ENTRYSHIFTINCREMENT ( 0x01 )
#define LCD_ENTRYSHIFTDECREMENT ( 0x00 )

// flags for function set
#define LCD_8BITMODE            ( 0x10 )
#define LCD_4BITMODE            ( 0x00 )
#define LCD_2LINE               ( 0x08 )
#define LCD_1LINE               ( 0x00 )
#define LCD_5x10DOTS            ( 0x04 )
#define LCD_5x8DOTS             ( 0x00 )

// flags for display on/off control
#define LCD_DISPLAYON           ( 0x04 )
#define LCD_DISPLAYOFF          ( 0x00 )
#define LCD_CURSORON            ( 0x02 )
#define LCD_CURSOROFF           ( 0x00 )
#define LCD_BLINKON             ( 0x01 )
#define LCD_BLINKOFF            ( 0x00 )

/*
 * Static variables
 */

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t drv_lcdSendCommand (drv_lcdDescriptor_t* pDevice, u8_t command);
GOS_STATIC gos_result_t drv_lcdSendData    (drv_lcdDescriptor_t* pDevice, u8_t data);

/*
 * Function: drv_lcdInit
 */
gos_result_t drv_lcdInit (void_t* pDevice)
{
    /*
     * Local variables.
     */
    gos_result_t lcdInitResult  = GOS_SUCCESS;
    u8_t         displayControl = 0u;

    /*
     * Function code.
     */
    if (pDevice != NULL)
    {
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(20));
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, 0x3));
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(5));
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, 0x3));
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(1));
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, 0x3));
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(1));
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, 0x2));
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(1));

    	// set # lines, font size, etc.
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS | LCD_2LINE));

    	// turn the display on with no cursor or blinking default
    	displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, LCD_DISPLAYCONTROL | displayControl));

    	// clear it off
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdClear((drv_lcdDescriptor_t*)pDevice));

    	// Initialize to default text direction (for roman languages).
    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT));

    	GOS_CONCAT_RESULT(lcdInitResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, LCD_RETURNHOME));  // set cursor position to zero
    	GOS_CONCAT_RESULT(lcdInitResult, gos_taskSleep(2));
    }
    else
    {
    	lcdInitResult = GOS_ERROR;
    }

    return lcdInitResult;
}

/*
 * Function: drv_lcdWriteString
 */
gos_result_t drv_lcdWriteString (void_t* pDevice, u8_t line, const char_t* str)
{
	/*
	 * Local variables.
	 */
	gos_result_t lcdWriteStringResult = GOS_SUCCESS;
	u8_t         index                = 0u;

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (line < 2) && (str != NULL))
	{
		GOS_CONCAT_RESULT(lcdWriteStringResult, drv_lcdSetCursor((drv_lcdDescriptor_t*)pDevice, line, 0));

		for (index = 0u; index < 16u && str[index]; index++)
		{
			GOS_CONCAT_RESULT(lcdWriteStringResult, drv_lcdSendData((drv_lcdDescriptor_t*)pDevice, (u8_t)str[index]));

			if (lcdWriteStringResult != GOS_SUCCESS)
			{
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		lcdWriteStringResult = GOS_ERROR;
	}

	return lcdWriteStringResult;
}

/*
 * Function: drv_lcdSetCursor
 */
gos_result_t drv_lcdSetCursor (void_t* pDevice, u8_t row, u8_t col)
{
	/*
	 * Local variables.
	 */
	gos_result_t lcdSetCursorResult = GOS_SUCCESS;
	u8_t         maskData           = (col) & 0x0F;

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (row < 2))
	{
		switch (row)
		{
			case 0:
			{
				maskData |= 0x80;
				lcdSetCursorResult = drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, maskData);
				break;
			}
			case 1:
			{
				maskData |= 0xC0;
				lcdSetCursorResult = drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, maskData);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else
	{
		lcdSetCursorResult = GOS_ERROR;
	}

	return lcdSetCursorResult;
}

/*
 * Function: drv_lcdClear
 */
gos_result_t drv_lcdClear (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	gos_result_t clearResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	if (pDevice != NULL)
	{
		GOS_CONCAT_RESULT(clearResult, drv_lcdSendCommand((drv_lcdDescriptor_t*)pDevice, LCD_CLEARDISPLAY));
		GOS_CONCAT_RESULT(clearResult, gos_taskSleep(2));
	}
	else
	{
		clearResult = GOS_ERROR;
	}

	return clearResult;
}

// TODO
GOS_STATIC gos_result_t drv_lcdSendCommand (drv_lcdDescriptor_t* pDevice, u8_t command)
{
	/*
	 * Local variables.
	 */
	gos_result_t lcdSendCmdResult = GOS_ERROR;
	u8_t data_u, data_l;
	u8_t data_t [4];

	/*
	 * Function code.
	 */
	data_u = ( command & 0xF0 );
	data_l = ( ( command << 4 ) & 0xF0 );
	data_t[0] = data_u | 0x0C;  //en=1, rs=0
	data_t[1] = data_u | 0x08;  //en=0, rs=0
	data_t[2] = data_l | 0x0C;  //en=1, rs=0
	data_t[3] = data_l | 0x08;  //en=0, rs=0

	lcdSendCmdResult = drv_i2cTransmitIT(
			pDevice->i2cInstance, (pDevice->deviceAddress) << 1, data_t, 4,
			pDevice->writeMutexTmo, pDevice->writeTriggerTmo);


	return lcdSendCmdResult;
}

// TODO
GOS_STATIC gos_result_t drv_lcdSendData (drv_lcdDescriptor_t* pDevice, u8_t data)
{
	/*
	 * Local variables.
	 */
	gos_result_t lcdSendDataResult = GOS_ERROR;
	u8_t data_u, data_l;
	u8_t data_t [4];

	/*
	 * Function code.
	 */
	data_u = ( data & 0xF0 );
	data_l = ( ( data << 4 ) & 0xF0 );
	data_t[0] = data_u | 0x0D;  //en=1, rs=1
	data_t[1] = data_u | 0x09;  //en=0, rs=1
	data_t[2] = data_l | 0x0D;  //en=1, rs=1
	data_t[3] = data_l | 0x09;  //en=0, rs=1

	lcdSendDataResult = drv_i2cTransmitIT(
			pDevice->i2cInstance, (pDevice->deviceAddress) << 1, data_t, 4,
			pDevice->writeMutexTmo, pDevice->writeTriggerTmo);

	(void_t) gos_taskSleep(1);

	return lcdSendDataResult;
}
