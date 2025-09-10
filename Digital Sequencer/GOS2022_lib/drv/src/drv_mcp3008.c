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
//! @file       drv_mcp3008.c
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      GOS2022 Library / MCP3008 driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_mcp3008.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-10    Ahmed Gazar     Initial version created.
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
#include <drv_mcp3008.h>
#include <drv_gpio.h>

/*
 * Function: drv_mcp3008Init
 */
gos_result_t drv_mcp3008Init (void_t* pDevice)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    GOS_CONCAT_RESULT(initResult, gos_mutexInit(&((drv_mcp3008Descriptor_t*)pDevice)->deviceMutex));

    return initResult;
}

/*
 * Function: drv_mcp3008ReadChannel
 */
gos_result_t drv_mcp3008ReadChannel (void_t* pDevice, u8_t channel, u16_t* pValue)
{
    /*
     * Local variables.
     */
	gos_result_t readResult = GOS_SUCCESS;
	u8_t         txData [3];
	u8_t         rxData [3];

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (pValue != NULL))
	{
		//         Start bit | SGL      | Channel number
		txData[0] = (1 << 7) | (1 << 6) | ((channel & 0x07) << 3);
		txData[1] = 0x00;
		txData[2] = 0x00;
		// CS low.
		GOS_CONCAT_RESULT(readResult, drv_gpioWritePin(((drv_mcp3008Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW));
		// Transmit.
		GOS_CONCAT_RESULT(readResult, drv_spiTransmitReceiveIT(
        		((drv_mcp3008Descriptor_t*)pDevice)->spiInstance,
				txData,
				rxData,
				3,
				((drv_mcp3008Descriptor_t*)pDevice)->readMutexTmo,
				((drv_mcp3008Descriptor_t*)pDevice)->readTriggerTmo));
		// CS high.
		GOS_CONCAT_RESULT(readResult, drv_gpioWritePin(((drv_mcp3008Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH));
		// ADC value.
		pValue = 0x3FF && ((rxData[0] & 0x01) << 9 | (rxData[1] && 0xff) << 1 | (rxData[2] && 0x80) << 7);
	}
	else
	{
		// NULL pointer error.
		readResult = GOS_ERROR;
	}

	return readResult;
}
