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
//! @file       svl_sdh.h
//! @author     Ahmed Gazar
//! @date       2024-12-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / Software Download Handler
//! @details    This component implements the software download service to store new application
//!             binary / or remove an existing one on an external device.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-24    Ahmed Gazar     Initial version created.
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
#ifndef SVL_SDH_H
#define SVL_SDH_H
/*
 * Includes
 */
#include <svl_pdh.h>

/*
 * Type definitions
 */
/**
 * SDH read/write function type.
 */
typedef gos_result_t (*svl_sdhReadWriteFunc_t)(u32_t address, u8_t* pData, u32_t size);

/**
 * SDH configuration structure.
 */
typedef struct
{
	svl_sdhReadWriteFunc_t readFunction;
	svl_sdhReadWriteFunc_t writeFunction;
}svl_sdhCfg_t;

/**
 * SDH binary descriptor type.
 */
typedef struct __attribute__((packed))
{
	char_t              name [32];
	u32_t               binaryLocation;
	svl_pdhBinaryInfo_t binaryInfo;
}svl_sdhBinaryDesc_t;

// TODO
gos_result_t svl_sdhInit (void_t);

// TODO
gos_result_t svl_sdhConfigure (svl_sdhCfg_t* pCfg);

#endif