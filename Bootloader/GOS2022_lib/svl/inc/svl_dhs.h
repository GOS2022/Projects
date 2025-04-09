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
//! @file       svl_dhs.h
//! @author     Ahmed Gazar
//! @date       2024-04-13
//! @version    1.0
//!
//! @brief      GOS2022 Library / Device Handler Service header.
//! @details    This component TODO
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-04-13    Ahmed Gazar     Initial version created.
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
#ifndef SVL_DHS_H
#define SVL_DHS_H
/*
 * Includes
 */
#include <gos.h>
#include <svl_cfg.h>

/*
 * Macros
 */
#define SVL_DHS_NAME_MAX_LENGTH     ( 32u )
#define SVL_DHS_DESC_MAX_LENGTH     ( 64u )
#define SVL_DHS_WRITE_FUNC_MAX_NUM  ( 4u )
#define SVL_DHS_READ_FUNC_MAX_NUM   ( 4u )

/*
 * Type definitions
 */
typedef gos_result_t (*svl_dhsReadWriteFunc_t)(u8_t, va_list args);

/**
 * Device states.
 */
typedef enum
{
	DHS_STATE_UNINITIALIZED,/**< DHS_STATE_UNINITIALIZED */
	DHS_STATE_HEALTHY,      /**< DHS_STATE_HEALTHY */
	DHS_STATE_ERROR         /**< DHS_STATE_ERROR */
}svl_dhsState_t;

/**
 * Device types.
 */
typedef enum
{
	DHS_TYPE_READONLY, /**< DHS_TYPE_READONLY */
	DHS_TYPE_WRITEONLY,/**< DHS_TYPE_WRITEONLY */
	DHS_TYPE_READWRITE,/**< DHS_TYPE_READWRITE */
	DHS_TYPE_VIRTUAL   /**< DHS_TYPE_VIRTUAL */
}svl_dhsType_t;

typedef u32_t svl_dhsDevId_t;

typedef struct __attribute__((packed))
{
	char_t                 name [SVL_DHS_NAME_MAX_LENGTH];
	char_t                 description [SVL_DHS_DESC_MAX_LENGTH];
	svl_dhsDevId_t         deviceId;
	void_t*                pDeviceDescriptor;
	svl_dhsType_t          deviceType;
	bool_t                 enabled;
	gos_result_t           (*pInitializer)(void_t*);
	void_t                 (*pErrorHandler)(void_t*);
	svl_dhsReadWriteFunc_t readFunctions  [SVL_DHS_READ_FUNC_MAX_NUM];
	svl_dhsReadWriteFunc_t writeFunctions [SVL_DHS_WRITE_FUNC_MAX_NUM];
	svl_dhsState_t         deviceState;
	u32_t                  errorCode;
	u32_t                  errorCounter;
	u32_t                  errorTolerance;
	u32_t                  readCounter;
	u32_t                  writeCounter;
}svl_dhsDevice_t;

// TODO
gos_result_t svl_dhsInit (
        void_t
        );

gos_result_t svl_dhsRegisterDevice (svl_dhsDevice_t* pDevice);

gos_result_t svl_dhsUnregisterDevice (svl_dhsDevId_t devId);

gos_result_t svl_dhsForceInitialize (svl_dhsDevId_t devId);

gos_result_t svl_dhsWriteDevice (svl_dhsDevId_t devId, u8_t functionIdx, u8_t length, ...);

gos_result_t svl_dhsReadDevice (svl_dhsDevId_t devId, u8_t functionIdx, u8_t length, ...);

gos_result_t svl_dhsGetNumOfDevices (u8_t* pNumber);

gos_result_t svl_dhsGetIndex (svl_dhsDevId_t deviceId, u8_t* pIndex);

gos_result_t svl_dhsGetDeviceData (u8_t index, svl_dhsDevice_t* pDevice);

gos_result_t svl_dhsEnableDevice (svl_dhsDevId_t devId);

gos_result_t svl_dhsDisableDevice (svl_dhsDevId_t devId);

#if 0
// TODO
gos_result_t svl_deviceHandlerRegisterDevice (
		svl_dhsDescriptor_t* pDevice
        );
#endif

#endif
