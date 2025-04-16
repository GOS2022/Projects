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
//! @details    For a more detailed description of this service, please refer to @ref svl_mdi.h
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
	SVL_MDI_SYSMON_MSG_MONITORING_DATA_NUM_GET_REQ  = 0x3001,
	SVL_MDI_SYSMON_MSG_MONITORING_DATA_NUM_GET_RESP = 0x3A01,
    SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_REQ      = 0x3002,
    SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_RESP     = 0x3A02
}svl_mdiSysmonMsgId_t;

/*
 * Static variables
 */
GOS_STATIC u16_t mdiVariableIndex = 0u;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_mdiNumReqCallback (void_t);
GOS_STATIC void_t svl_mdiReqCallback (void_t);

/**
 * Sysmon MDI variable number request message.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t mdiNumRequestMsg =
{
	.callback        = svl_mdiNumReqCallback,
	.messageId       = SVL_MDI_SYSMON_MSG_MONITORING_DATA_NUM_GET_REQ,
	.payloadSize     = 0,
	.protocolVersion = 1,
	.payload         = NULL
};

/**
 * Sysmon MDI variables request message.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t mdiRequestMsg =
{
	.callback        = svl_mdiReqCallback,
	.messageId       = SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_REQ,
	.payloadSize     = sizeof(u16_t),
	.protocolVersion = 1,
	.payload         = &mdiVariableIndex
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
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&mdiNumRequestMsg));
	GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&mdiRequestMsg));

	return initResult;
}

/**
 * @brief   Serves the #SVL_MDI_SYSMON_MSG_MONITORING_DATA_NUM_GET_REQ message.
 * @details Sends out the number of MDI entries in @ref mdiVariables.
 *
 * @return  -
 */
GOS_STATIC void_t svl_mdiNumReqCallback (void_t)
{
	/*
	 * Local variables.
	 */
	u16_t varNum = 0u;

	/*
	 * Function code.
	 */
	varNum = mdiVariablesSize / sizeof(svl_mdiVariable_t);

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			SVL_MDI_SYSMON_MSG_MONITORING_DATA_NUM_GET_RESP,
			(void_t*)&varNum,
			sizeof(varNum),
			0xFFFF);
}

/**
 * @brief   Serves the #SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_REQ message.
 * @details Sends out the MDI entry in @ref mdiVariables determined by its index.
 *
 * @return  -
 */
GOS_STATIC void_t svl_mdiReqCallback (void_t)
{
	/*
	 * Function code.
	 */
	if ((mdiVariables != NULL) && (mdiVariablesSize > 0u) &&
		(mdiVariableIndex < (mdiVariablesSize / sizeof(svl_mdiVariable_t))))
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				SVL_MDI_SYSMON_MSG_MONITORING_DATA_GET_RESP,
				(void_t*)&mdiVariables[mdiVariableIndex],
				sizeof(svl_mdiVariable_t),
				0xFFFF);
	}
	else
	{
		// Nothing to send.
	}
}
