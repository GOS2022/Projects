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
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       svl_ipl.h
//! @author     Ahmed Gazar
//! @date       2024-12-15
//! @version    1.0
//!
//! @brief      GOS2022 Library / Inter-Processor Link header.
//! @details    This component implements the communication protocol between the WeMos D1 mini and
//!             STM32 on the GOS main board.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-15    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
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
#ifndef SVL_IPL_H
#define SVL_IPL_H
/*
 * Includes
 */
#include <gos.h>
#include <bld_com.h>

/*
 * Type definitions
 */
/**
 * IPL transmit function type.
 */
typedef gos_result_t (*svl_iplTransmitFunction)(u8_t* pData, u32_t dataSize, u32_t timeout);

/**
 * IPL receive function type.
 */
typedef gos_result_t (*svl_iplReceiveFunction)(u8_t* pData, u32_t dataSize, u32_t timeout);

/**
 * IPL user-defined message callback funciton type.
 */
typedef void_t (*svl_iplCallback_t)(u8_t* pData, u32_t size, u32_t crc);

/**
 * IPL user message descriptor type.
 */
typedef struct
{
	u32_t             msgId;
	svl_iplCallback_t callback;
}svl_iplUserMsgDesc_t;

// TODO
gos_result_t svl_iplInit (void_t);

// TODO
gos_result_t svl_iplRegisterUserMsg (svl_iplUserMsgDesc_t* pUserMsgDesc);

// TODO
gos_result_t svl_iplConfigure (svl_iplTransmitFunction transmitFunc, svl_iplReceiveFunction receiveFunc);

#endif
