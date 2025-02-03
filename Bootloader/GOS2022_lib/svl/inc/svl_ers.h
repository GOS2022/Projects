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
//! @file       svl_ers.h
//! @author     Ahmed Gazar
//! @date       2025-01-29
//! @version    1.0
//!
//! @brief      GOS2022 Library / Event Reporting Service header.
//! @details    This component implements the generic event reporting to create, read, and clear
//!             events in a user-defined non-volatile memory.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-01-29    Ahmed Gazar     Initial version created.
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
#ifndef SVL_ERS_H
#define SVL_ERS_H
/*
 * Includes
 */
#include <gos.h>
#include <svl_cfg.h>

/*
 * Type definitions
 */
/**
 * Event descriptor type.
 */
typedef struct __attribute__((packed))
{
	char_t     description [SVL_ERS_DESC_LENGTH]; //!< Event description.
	gos_time_t timeStamp;                         //!< Time stamp (auto-generated).
	u32_t      trigger;                           //!< Optional trigger code.
	u8_t       eventData [SVL_ERS_DATA_SIZE];     //!< Optional event data.
}svl_ersEventDesc_t;

/**
 * ERS read/write function type.
 */
typedef gos_result_t (*svl_ersReadWriteFunc_t)(u32_t address, u8_t* pData, u16_t size);

/**
 * ERS configuration structure.
 */
typedef struct
{
	svl_ersReadWriteFunc_t readFunction;  //!< Read function.
	svl_ersReadWriteFunc_t writeFunction; //!< Write function.
}svl_ersCfg_t;

/**
 * @brief   Initializes the ERS.
 * @details Registers the sysmon messages for the service, and initializes
 *          the internal mutex.
 *
 * @return  Result of initialization.
 *
 * @retval  GOS_SUCCESS : Initialization successful.
 * @retval  GOS_ERROR   : Sysmon registration error or mutex initialization error.
 */
gos_result_t svl_ersInit (void_t);

/**
 * @brief   Configures the ERS.
 * @details Saves the desired read and write functions.
 *
 * @param   pCfg : Pointer to the desired configuration variable.
 *
 * @return  Result of configuration.
 *
 * @retval  GOS_SUCCESS : Configuration successful.
 * @retval  GOS_ERROR   : NULL pointer.
 */
gos_result_t svl_ersConfigure (svl_ersCfg_t* pCfg);

/**
 * @brief   Creates an event.
 * @details Creates a time stamp, copies the event data, and stores the
 *          event in the non-volatile memory.
 *
 * @param   index      : Index of event in the user-defined array.
 * @param   trigger    : Trigger code (optional).
 * @param   pEventData : Event data bytes (optional).
 *
 * @return  Result of event creation.
 *
 * @retval  GOS_SUCCESS : Event creation successful.
 * @retval  GOS_ERROR   : Missing write function, mutex, or driver error.
 */
gos_result_t svl_ersCreate (u16_t index, u32_t trigger, u8_t* pEventData);

/**
 * @brief   Reads an event.
 * @details Copies the event with the required index from the non-volatile
 *          memory to the target buffer.
 *
 * @param   index   : Index of event in non-volatile memory.
 * @param   pTarget : Buffer to store the event in.
 *
 * @return  Result of event reading.
 *
 * @retval  GOS_SUCCESS : Event reading successful.
 * @retval  GOS_ERROR   : Missing read function, mutex, or driver error.
 */
gos_result_t svl_ersRead (u16_t index, u8_t* pTarget);

/**
 * @brief   Returns the number of entries.
 * @details Reads the number of entries from the non-volatile memory.
 *
 * @param   pNumOfEntries : Pointer to a variable to store the number of entries in.
 *
 * @return  Result of entry number getting.
 *
 * @retval  GOS_SUCCESS : Entry number getting successful.
 * @retval  GOS_ERROR   : Missing read function, mutex, or driver error.
 */
gos_result_t svl_ersGetNumOfEntries (u32_t* pNumOfEntries);

/**
 * @brief   Clears the events.
 * @details Sets the event number to zero.
 *
 * @return  Result of event clearing.
 *
 * @retval  GOS_SUCCESS : Event clearing successful.
 * @retval  GOS_ERROR   : Missing write function or driver error.
 */
gos_result_t svl_ersClearEvents (void_t);

#endif
