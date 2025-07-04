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
/**
 * Device name maximum length.
 */
#define SVL_DHS_NAME_MAX_LENGTH     ( 32u )

/**
 * Device description maximum length.
 */
#define SVL_DHS_DESC_MAX_LENGTH     ( 64u )

/**
 * Maximum number of write functions.
 */
#define SVL_DHS_WRITE_FUNC_MAX_NUM  ( 4u )

/**
 * Maximum number of read functions.
 */
#define SVL_DHS_READ_FUNC_MAX_NUM   ( 4u )

/**
 * Macro for declaring a read function.
 */
#define SVL_DHS_READ_FUNC_DECL(funcName) GOS_STATIC gos_result_t funcName (u32_t address, u8_t* pData, u32_t size)

/**
 * Macro for defining a read function.
 */
#define SVL_DHS_READ_FUNC_DEF(funcName, devDescr, funcIdx, len)                                                                                                                  \
SVL_DHS_READ_FUNC_DECL(funcName)                                                                                 \
{                                                                                                                \
    return svl_dhsReadDevice(devDescr.deviceId, funcIdx, len, devDescr.pDeviceDescriptor, address, pData, size); \
}

/**
 * Macro for declaring a write function.
 */
#define SVL_DHS_WRITE_FUNC_DECL(funcName) GOS_STATIC gos_result_t funcName (u32_t address, u8_t* pData, u32_t size)

/**
 * Macro for defining a write function.
 */
#define SVL_DHS_WRITE_FUNC_DEF(funcName, devDescr, funcIdx, len)                                                                                                                  \
SVL_DHS_WRITE_FUNC_DECL(funcName)                                                                                 \
{                                                                                                                 \
    return svl_dhsWriteDevice(devDescr.deviceId, funcIdx, len, devDescr.pDeviceDescriptor, address, pData, size); \
}

/**
 * Macro for declaring read function wrapper.
 */
#define SVL_DHS_READ_FUNC_WRAPPER_DECL(funcName) GOS_STATIC gos_result_t funcName (u8_t params, va_list args)

/**
 * Macro for declaring write function wrapper.
 */
#define SVL_DHS_WRITE_FUNC_WRAPPER_DECL(funcName) GOS_STATIC gos_result_t funcName (u8_t params, va_list args)


/*
 * Type definitions
 */
typedef gos_result_t (*svl_dhsReadWriteFunc_t)(u8_t, va_list args);

/**
 * Device states.
 */
typedef enum
{
	DHS_STATE_UNINITIALIZED = 0,  //!< Device uninitialized.
	DHS_STATE_HEALTHY       = 1,  //!< Device healthy (no errors).
	DHS_STATE_WARNING       = 2,  //!< Device healthy (errors within tolerance).
	DHS_STATE_ERROR         = 3   //!< Device in error (errors reached tolerance).
}svl_dhsState_t;

/**
 * Device types.
 */
typedef enum
{
	DHS_TYPE_READONLY       = 0,  //!< Read only (only read function).
	DHS_TYPE_WRITEONLY      = 1,  //!< Write only (only write function).
	DHS_TYPE_READWRITE      = 2,  //!< Read/write (both read and write function).
	DHS_TYPE_VIRTUAL        = 3,  //!< Virtual (neither read nor write function).
}svl_dhsType_t;

/**
 * Device ID type.
 */
typedef u32_t svl_dhsDevId_t;

/**
 * Device structure that contains all device data.
 */
typedef struct __attribute__((packed))
{
	char_t                 name [SVL_DHS_NAME_MAX_LENGTH];              //!< Device name.
	char_t                 description [SVL_DHS_DESC_MAX_LENGTH];       //!< Description.
	svl_dhsDevId_t         deviceId;                                    //!< Device ID.
	void_t*                pDeviceDescriptor;                           //!< Specific device descriptor.
	svl_dhsType_t          deviceType;                                  //!< Device type.
	bool_t                 enabled;                                     //!< Enabled flag.
	gos_result_t           (*pInitializer)(void_t*);                    //!< Initializer function pointer.
	void_t                 (*pErrorHandler)(void_t*);                   //!< Error handler function pointer.
	svl_dhsReadWriteFunc_t readFunctions  [SVL_DHS_READ_FUNC_MAX_NUM];  //!< Read function pointer array.
	svl_dhsReadWriteFunc_t writeFunctions [SVL_DHS_WRITE_FUNC_MAX_NUM]; //!< Write function pointer array.
	svl_dhsState_t         deviceState;                                 //!< Device state.
	u32_t                  errorCode;                                   //!< Error code.
	u32_t                  errorCounter;                                //!< Error counter.
	u32_t                  errorTolerance;                              //!< Error tolerance.
	u32_t                  readCounter;                                 //!< Read counter.
	u32_t                  writeCounter;                                //!< Write counter.
}svl_dhsDevice_t;

/**
 * @brief   Initializes the DHS service.
 * @details Registers the DHS daemon and registers the sysmon callbacks.
 *
 * @return  Result of service initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   One of the following errors occured:
 *                       - Daemon task registration failed
 *                       - Sysmon user message callback registration failed
 */
gos_result_t svl_dhsInit (
        void_t
        );

// TODO
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

#endif
