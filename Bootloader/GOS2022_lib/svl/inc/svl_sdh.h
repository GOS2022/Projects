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
 * Macros
 */
/**
 * SDH binary name length.
 */
#define SVL_SDH_BINARY_NAME_LENGTH ( 32u )

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
	svl_sdhReadWriteFunc_t readFunction;  //!< Read function pointer.
	svl_sdhReadWriteFunc_t writeFunction; //!< Write function pointer.
}svl_sdhCfg_t;

/**
 * SDH binary descriptor type.
 */
typedef struct __attribute__((packed))
{
	char_t              name [SVL_SDH_BINARY_NAME_LENGTH]; //!< Name of the binary.
	u32_t               binaryLocation;                    //!< Memory address of the binary.
	svl_pdhBinaryInfo_t binaryInfo;                        //!< Binary info.
}svl_sdhBinaryDesc_t;

// TODO
gos_result_t svl_sdhInit (void_t);

// TODO
gos_result_t svl_sdhConfigure (svl_sdhCfg_t* pCfg);

// TODO
gos_result_t svl_sdhGetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc);

// TODO
gos_result_t svl_sdhSetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc);

// TODO
gos_result_t svl_sdhReadBytesFromMemory (u32_t address, u8_t* pBuffer, u32_t size);

#endif
