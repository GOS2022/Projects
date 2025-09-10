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
//! @file       svl_sysmon.h
//! @author     Ahmed Gazar
//! @date       2025-06-18
//! @version    1.0
//!
//! @brief      GOS2022 Library / System Monitoring Service header.
//! @details    This service is used to send and receive system information to an external client
//!             such as a PC tool.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-06-18    Ahmed Gazar     Initial version created
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
#ifndef SVL_SYSMON_H
#define SVL_SYSMON_H
/*
 * Includes
 */
#include <gos.h>

/*
 * Type definitions
 */
/**
 * Sysmon service configuration structure type.
 */
typedef struct
{
    gos_gcpChannelNumber_t    wiredChannelNumber;      //!< GCP channel number for wired connection.
    gos_gcpTransmitFunction_t wiredTxFunction;         //!< TX function for wired connection.
    gos_gcpReceiveFunction_t  wiredRxFunction;         //!< RX function for wired connection.
    gos_gcpChannelNumber_t    wirelessChannelNumber;   //!< GCP channel number for wireless connection.
    gos_gcpTransmitFunction_t wirelessTxFunction;      //!< TX function for wireless connection.
    gos_gcpReceiveFunction_t  wirelessRxFunction;      //!< RX function for wireless connection.
}svl_sysmonServiceConfig_t;

/**
 * Sysmon message received callback function type.
 */
typedef void_t (*svl_sysmonMessageReceivedCallback) (gos_gcpChannelNumber_t);

/**
 * Sysmon user message descriptor.
 */
typedef struct
{
	u16_t                             messageId;       //!< Message ID.
	void_t*                           payload;         //!< Pointer to payload target.
	u32_t                             payloadSize;     //!< Size of payload.
	svl_sysmonMessageReceivedCallback callback;        //!< Callback function pointer.
}svl_sysmonUserMessageDescriptor_t;

/**
 * @brief   Initializes the system monitoring service.
 * @details Initializes the internal user message array, registers the GCP
 *          physical drivers, the daemon tasks in the kernel, and initializes
 *          the internal mutex.
 *
 * @return  Result of initialization.
 *
 * @retval  #GOS_SUCCESS Sysmon initialization successful.
 * @retval  #GOS_ERROR   Either of the initialization activities failed.
 */
gos_result_t svl_sysmonInit (void_t);

/**
 * @brief   This function registers a custom user sysmon message.
 * @details Registers a custom sysmon message given by its ID. The message
 *          will only be processed if the required ID is not an existing sysmon ID.
 *          When a message is received with the given ID, the message content
 *          will be copied into the buffer defined in the descriptor structure, and
 *          the registered callback function will be called.
 *
 *          Recommended ID range: 0x6000 ... 0x9999.
 *
 * @param[in] pDesc Pointer to a sysmon user message descriptor.
 *
 * @return  Result of registration.
 *
 * @retval  #GOS_SUCCESS User message registered successfully.
 * @retval  #GOS_ERROR   Descriptor or callback is NULL or maximum number of user
 *                       messages has been reached.
 */
gos_result_t svl_sysmonRegisterUserMessage (svl_sysmonUserMessageDescriptor_t* pDesc);

#endif
