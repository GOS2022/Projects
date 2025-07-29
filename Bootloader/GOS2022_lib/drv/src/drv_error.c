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
//! @file       drv_error.c
//! @author     Ahmed Gazar
//! @date       2025-07-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / Driver error source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_error.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-07-24    Ahmed Gazar     Initial version created.
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
#include <drv_error.h>

/*
 * Function: drv_errorGetDiagData
 */
gos_result_t drv_errorGetDiagData (drv_diagData_t* pDiag)
{
	/*
	 * Local variables.
	 */
	gos_result_t getDiagResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(getDiagResult, drv_uartGetDiagData(&(pDiag->uartDiag)));
	GOS_CONCAT_RESULT(getDiagResult, drv_tmrGetDiagData(&(pDiag->tmrDiag)));
	GOS_CONCAT_RESULT(getDiagResult, drv_spiGetDiagData(&(pDiag->spiDiag)));
	GOS_CONCAT_RESULT(getDiagResult, drv_i2cGetDiagData(&(pDiag->i2cDiag)));

	return getDiagResult;
}
