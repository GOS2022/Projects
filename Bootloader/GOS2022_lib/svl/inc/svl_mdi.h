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
//! @file       svl_mdi.h
//! @author     Ahmed Gazar
//! @date       2025-01-27
//! @version    1.0
//!
//! @brief      GOS2022 Library / Monitoring Data Interface
//! @details    This component implements the generic monitoring data interface which makes it
//!             possible to define variables that are accessible via sysmon.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-01-27    Ahmed Gazar     Initial version created.
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
#ifndef SVL_MDI_H
#define SVL_MDI_H
/*
 * Includes
 */
#include <gos.h>
#include <svl_cfg.h>

/*
 * Type definitions
 */
/**
 * MDI value converter type.
 */
typedef union
{
	u8_t    byteValue;      //!< Byte value.
	u16_t   wordValue;      //!< Word value.
	u32_t   lwordValue;     //!< Long word value.
	float_t floatValue;     //!< Float value.
	u8_t    arrayValue [4]; //!< 4-byte array value.
}svl_mdiValueConvert_t;

/**
 * MDI supported value types.
 */
typedef enum
{
	MDI_VAL_TYPE_BYTE  = 0, //!< 8-bit unsigned.
	MDI_VAL_TYPE_WORD  = 1, //!< 16-bit unsigned.
	MDI_VAL_TYPE_LWORD = 2, //!< 32-bit unsigned.
	MDI_VAL_TYPE_FLOAT = 3  //!< Float.
}svl_mdiValueType_t;

/**
 * MDI variable.
 */
typedef struct __attribute__((packed))
{
	char_t                name [SVL_MDI_VAR_NAME_LENGTH]; //!< Variable name.
	svl_mdiValueType_t    type;                           //!< Variable type.
	svl_mdiValueConvert_t value;                          //!< Value.
}svl_mdiVariable_t;

/**
 * @brief   Initializes the MDI.
 * @details Registers the sysmon user callback messages to serve
 *          requests.
 *
 * @return  Result of initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   Sysmon user callback registration failed.
 */
gos_result_t svl_mdiInit (void_t);

#endif
