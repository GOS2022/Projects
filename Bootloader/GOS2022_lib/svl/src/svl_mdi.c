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
//! @file       svl_mdi.c
//! @author     Ahmed Gazar
//! @date       2025-01-27
//! @version    1.0
//!
//! @brief      GOS2022 Library / Monitoring Data Interface source.
//! @details    For a more detailed description of this driver, please refer to @ref svl_mdi.h
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
/*
 * Includes
 */
#include <svl_mdi.h>

/*
 * Type definitions
 */
typedef enum
{
    SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_REQ  = 0x3001,
    SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_RESP = 0x3A01
}svl_mdiSysmonMsgId_t;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_mdiReqCallback (void_t);

/**
 * Sysmon MDI variables request message.
 */
gos_sysmonUserMessageDescriptor_t mdiRequestMsg =
{
	.callback        = svl_mdiReqCallback,
	.messageId       = SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_REQ,
	.payloadSize     = 0u,
	.protocolVersion = 1,
	.payload         = NULL
};

/**
 * Monitoring variable array.
 */
GOS_EXTERN svl_mdiVariable_t mdiVariables [];

/**
 * Size of the monitoring variable array.
 */
GOS_EXTERN u32_t             mdiVariablesSize;

/*
 * Function: svl_mdiInit
 */
gos_result_t svl_mdiInit (void_t)
{
	/*
	 * Function code.
	 */
	return gos_sysmonRegisterUserMessage(&mdiRequestMsg);
}

/*
 * TODO
 */
GOS_STATIC void_t svl_mdiReqCallback (void_t)
{
	/*
	 * Function code.
	 */
	if (mdiVariables != NULL && mdiVariablesSize > 0u)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_RESP,
				(void_t*)mdiVariables,
				mdiVariablesSize,
				0xFFFF);
	}
	else
	{
		// Nothing to send.
	}
}
