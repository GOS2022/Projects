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
//! @file       svl_sysmon.c
//! @author     Ahmed Gazar
//! @date       2025-06-18
//! @version    1.0
//!
//! @brief      GOS2022 Library / System Monitoring Service source.
//! @details    For a more detailed description of this service, please refer to @ref svl_sysmon.h
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
/*
 * Includes
 */
#include <svl_sysmon.h>
#include <svl_cfg.h>
#include <string.h>

/*
 * Type definitions
 */
/**
 * System monitoring message index enumerator.
 */
typedef enum
{
    SVL_SYSMON_MSG_UNKNOWN                   = 0,        //!< Unknown message LUT index.
    SVL_SYSMON_MSG_PING,                                 //!< Ping message LUT index.
    SVL_SYSMON_MSG_PING_RESP,                            //!< Ping response message LUT index.
    SVL_SYSMON_MSG_CPU_USAGE_GET,                        //!< CPU usage get message LUT index.
    SVL_SYSMON_MSG_CPU_USAGE_GET_RESP,                   //!< CPU usage get response message LUT index.
    SVL_SYSMON_MSG_TASK_GET_DATA,                        //!< Task data get message LUT index.
    SVL_SYSMON_MSG_TASK_GET_DATA_RESP,                   //!< Task data get response message LUT index.
    SVL_SYSMON_MSG_TASK_GET_VAR_DATA,                    //!< Task variable data get message LUT index.
    SVL_SYSMON_MSG_TASK_GET_VAR_DATA_RESP,               //!< Task variable data get response message LUT index.
    SVL_SYSMON_MSG_TASK_MODIFY_STATE,                    //!< Task modify message LUT index.
    SVL_SYSMON_MSG_TASK_MODIFY_STATE_RESP,               //!< Task modify response message LUT index.
    SVL_SYSMON_MSG_SYSRUNTIME_GET,                       //!< System runtime get message LUT index.
    SVL_SYSMON_MSG_SYSRUNTIME_GET_RESP,                  //!< System runtime get response message LUT index.
    SVL_SYSMON_MSG_SYSTIME_SET,                          //!< System time set message LUT index.
    SVL_SYSMON_MSG_SYSTIME_SET_RESP,                     //!< System time set response message LUT index.
    SVL_SYSMON_MSG_RESET_REQ,                            //!< System reset message LUT index.
    SVL_SYSMON_MSG_NUM_OF_MESSAGES,                      //!< Number of messages.
}svl_sysmonMessageEnum_t;

/**
 * System monitoring message ID enum.
 */
typedef enum
{
    SVL_SYSMON_MSG_UNKNOWN_ID                = 0,        //!< Unknown message ID.
    SVL_SYSMON_MSG_INVALID_ID                = 0xFFFF,   //!< Invalid message ID.
    SVL_SYSMON_MSG_PING_ID                   = 0x0001,   //!< Ping message ID.
    SVL_SYSMON_MSG_PING_RESP_ID              = 0x0A01,   //!< Ping response message ID.
    SVL_SYSMON_MSG_CPU_USAGE_GET_ID          = 0x0002,   //!< CPU usage get message ID.
    SVL_SYSMON_MSG_CPU_USAGE_GET_RESP_ID     = 0x0A02,   //!< CPU usage get response message ID.
    SVL_SYSMON_MSG_TASK_GET_DATA_ID          = 0x0003,   //!< Task data get message ID.
    SVL_SYSMON_MSG_TASK_GET_DATA_RESP_ID     = 0x0A03,   //!< Task data get response message ID.
    SVL_SYSMON_MSG_TASK_GET_VAR_DATA_ID      = 0x0004,   //!< Task variable data get message ID.
    SVL_SYSMON_MSG_TASK_GET_VAR_DATA_RESP_ID = 0x0A04,   //!< Task variable data get response message ID.
    SVL_SYSMON_MSG_TASK_MODIFY_STATE_ID      = 0x0005,   //!< Task modify state message ID.
    SVL_SYSMON_MSG_TASK_MODIFY_STATE_RESP_ID = 0x0A05,   //!< Task modify state response ID.
    SVL_SYSMON_MSG_SYSRUNTIME_GET_ID         = 0x0006,   //!< System runtime get message ID.
    SVL_SYSMON_MSG_SYSRUNTIME_GET_RESP_ID    = 0x0A06,   //!< System runtime get response message ID.
    SVL_SYSMON_MSG_SYSTIME_SET_ID            = 0x0007,   //!< System time set message ID.
    SVL_SYSMON_MSG_SYSTIME_SET_RESP_ID       = 0x0A07,   //!< System time set response ID.
    SVL_SYSMON_MSG_RESET_REQ_ID              = 0x0FFF,   //!< System reset request ID.
}svl_sysmonMessageId_t;

/**
 * Message result enum.
 */
typedef enum
{
    SVL_SYSMON_MSG_RES_OK          = 40,                 //!< Message result OK.
    SVL_SYSMON_MSG_RES_ERROR       = 99,                 //!< Message result ERROR.
    SVL_SYSMON_MSG_INV_PAYLOAD_CRC = 28                  //!< Invalid payload CRC.
}svl_sysmonMessageResult_t;

/**
 * State modification enum.
 */
typedef enum
{
    SVL_SYSMON_TASK_MOD_TYPE_SUSPEND = 12,               //!< Task suspend.
    SVL_SYSMON_TASK_MOD_TYPE_RESUME  = 34,               //!< Task resume.
    SVL_SYSMON_TASK_MOD_TYPE_DELETE  = 49,               //!< Task delete.
    SVL_SYSMON_TASK_MOD_TYPE_BLOCK   = 52,               //!< Task block.
    SVL_SYSMON_TASK_MOD_TYPE_UNBLOCK = 63,               //!< Task unblock.
    SVL_SYSMON_TASK_MOD_TYPE_WAKEUP  = 74                //!< Task wakeup.
}svl_sysmonTaskModifyType_t;

/**
 * Task data message structure.
 */
typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskPrio_t           taskOriginalPriority;       //!< Task original priority.
    gos_taskPrivilegeLevel_t taskPrivilegeLevel;         //!< Task privilege level.
    gos_taskName_t           taskName;                   //!< Task name.
    gos_tid_t                taskId;                     //!< Task ID (internal).
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_taskStackSize_t      taskStackSize;              //!< Task stack size.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageLimit;          //!< Task CPU usage limit in [% x 100].
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task max. stack usage.
}svl_sysmonTaskData_t;

/**
 * Task variable data message structure.
 */
typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task stack usage.
}svl_sysmonTaskVariableData_t;

/**
 * @defgroup SysmonMsgStructs System monitoring message structures
 * @{
 */
/**
 * Ping message structure.
 */
typedef struct __attribute__((packed))
{
    svl_sysmonMessageResult_t messageResult;             //!< Message result.
}svl_sysmonPingMessage_t;


/**
 * CPU usage message structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t messageResult;             //!< Message result.
    u16_t                     cpuUsage;                  //!< CPU usage x100[%].
}svl_sysmonCpuUsageMessage_t;

/**
 * Task data get message structure.
 */
typedef struct __attribute__((packed))
{
    u16_t taskIndex;                                     //!< Task index.
}svl_sysmonTaskDataGetMessage_t;

/**
 * Task data message structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t messageResult;             //!< Message result.
    svl_sysmonTaskData_t      taskData;                  //!< Task data.
}svl_sysmonTaskDataMessage_t;

/**
 * Task variable data message structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t    messageResult;          //!< Message result.
	svl_sysmonTaskVariableData_t taskVariableData;       //!< Task variable data.
}svl_sysmonTaskVariableDataMessage_t;

/**
 * Task modify message structure.
 */
typedef struct __attribute__((packed))
{
    u16_t                      taskIndex;                //!< Task index.
    svl_sysmonTaskModifyType_t modificationType;         //!< Task modification type.
    u32_t                      param;                    //!< Parameter for functions where timeout is required.
}svl_sysmonTaskModifyMessage_t;

/**
 * Task modify message result structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t messageResult;             //!< Message result.
}svl_sysmonTaskModifyResultMessage_t;

/**
 * System runtime get message result structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t messageResult;             //!< Message result.
    gos_runtime_t             sysRuntime;                //!< System runtime.
}svl_sysmonSysruntimeGetResultMessage_t;

/**
 * System time set message structure.
 */
typedef struct __attribute__((packed))
{
    gos_time_t desiredSystemTime;                        //!< Desired system time.
}svl_sysmonSystimeSetMessage_t;

/**
 * System time set message result structure.
 */
typedef struct __attribute__((packed))
{
	svl_sysmonMessageResult_t messageResult;             //!< Message result.
}svl_sysmonSystimeSetResultMessage_t;
/**
 * @}
 */

/**
 * Message handler function type.
 */
typedef void_t (*svl_sysmonMessageHandler_t)(gos_gcpChannelNumber_t, void_t*, svl_sysmonMessageEnum_t);

/**
 * Look-up table entry structure.
 */
typedef struct
{
    svl_sysmonMessageId_t      messageId;                //!< Message ID.
    void_t*                    pMessagePayload;          //!< Payload pointer.
    u16_t                      payloadSize;              //!< Payload size.
    svl_sysmonMessageHandler_t pHandler;                 //!< Handler function pointer.
}svl_sysmonLut_t;

/*
 * External variables
 */
/**
 * Pre-defined event descriptors.
 */
GOS_EXTERN GOS_CONST svl_sysmonServiceConfig_t    sysmonConfig;

/*
 * Static variables
 */
/**
 * Sysmon mutex.
 */
GOS_STATIC gos_mutex_t                            sysmonMutex;

/**
 * Wired receive buffer.
 */
GOS_STATIC u8_t                                   wiredRxBuffer    [SVL_SYSMON_WIRED_RX_BUFF_SIZE];

/**
 * Wireless receive buffer.
 */
GOS_STATIC u8_t                                   wirelessRxBuffer [SVL_SYSMON_WIRELESS_RX_BUFF_SIZE];

/**
 * Ping message structure.
 */
GOS_STATIC svl_sysmonPingMessage_t                pingMessage                = {0};

/**
 * CPU load message structure.
 */
GOS_STATIC svl_sysmonCpuUsageMessage_t            cpuMessage                 = {0};

/**
 * Task data get message structure.
 */
GOS_STATIC svl_sysmonTaskDataGetMessage_t         taskDataGetMsg             = {0};

/**
 * Task data message structure.
 */
GOS_STATIC svl_sysmonTaskDataMessage_t            taskDataMsg                = {0};

/**
 * Task variable data message structure.
 */
GOS_STATIC svl_sysmonTaskVariableDataMessage_t    taskVariableDataMsg        = {0};

/**
 * Task descriptor structure.
 */
GOS_STATIC gos_taskDescriptor_t                   taskDesc                   = {0};

/**
 * Task modify message.
 */
GOS_STATIC svl_sysmonTaskModifyMessage_t          taskModifyMessage          = {0};

/**
 * Task modify result message.
 */
GOS_STATIC svl_sysmonTaskModifyResultMessage_t    taskModifyResultMessage    = {0};

/**
 * System runtime get message.
 */
GOS_STATIC svl_sysmonSysruntimeGetResultMessage_t sysRuntimeGetResultMessage = {0};

/**
 * System time set message.
 */
GOS_STATIC svl_sysmonSystimeSetMessage_t          sysTimeSetMessage          = {0};

/**
 * System time set result message.
 */
GOS_STATIC svl_sysmonSystimeSetResultMessage_t    sysTimeSetResultMessage    = {0};

/**
 * Sysmon user messages.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t      userMessages [SVL_SYSMON_MAX_USER_MESSAGES];

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t              svl_sysmonFillTaskData              (svl_sysmonTaskData_t* pSysmonTaskData, gos_taskDescriptor_t* pTaskData);
GOS_STATIC gos_result_t              svl_sysmonFillTskVariableData       (svl_sysmonTaskVariableData_t* pSysmonTaskVariableData, gos_taskDescriptor_t* pTaskData);
GOS_STATIC svl_sysmonMessageEnum_t   svl_sysmonGetLutIndex               (svl_sysmonMessageId_t messageId);
GOS_STATIC svl_sysmonMessageResult_t svl_sysmonCheckMessage              (void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonSendResponse              (gos_gcpChannelNumber_t gcpChannel, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandlePingRequest         (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleCpuUsageGet         (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleTaskDataGet         (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleTaskVariableDataGet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleTaskModification    (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleSysRuntimeGet       (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleSystimeSet          (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonHandleResetRequest        (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex);
GOS_STATIC void_t                    svl_sysmonWiredDaemonTask           (void_t);
GOS_STATIC void_t                    svl_sysmonWirelessDaemonTask        (void_t);

/**
 * Sysmon wired daemon task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t sysmonWiredDaemonTaskDesc =
{
    .taskFunction        = svl_sysmonWiredDaemonTask,
    .taskName            = "svl_sysmon_wired_daemon",
    .taskPriority        = SVL_SYSMON_WIRED_DAEMON_PRIO,
    .taskStackSize       = SVL_SYSMON_WIRED_DAEMON_STACK,
    .taskPrivilegeLevel  = GOS_TASK_PRIVILEGE_KERNEL
};

/**
 * Sysmon wireless daemon task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t sysmonWirelessDaemonTaskDesc =
{
    .taskFunction        = svl_sysmonWirelessDaemonTask,
    .taskName            = "svl_sysmon_wireless_daemon",
    .taskPriority        = SVL_SYSMON_WIRELESS_DAEMON_PRIO,
    .taskStackSize       = SVL_SYSMON_WIRELESS_DAEMON_STACK,
    .taskPrivilegeLevel  = GOS_TASK_PRIVILEGE_KERNEL
};

/**
 * Sysmon look-up table.
 */
GOS_STATIC GOS_CONST svl_sysmonLut_t sysmonLut [SVL_SYSMON_MSG_NUM_OF_MESSAGES] =
{
    [SVL_SYSMON_MSG_UNKNOWN]                =
    {
        .messageId       = SVL_SYSMON_MSG_UNKNOWN_ID,
        .pMessagePayload = NULL,
        .payloadSize     = 0u
    },
    [SVL_SYSMON_MSG_PING]                   =
    {
        .messageId       = SVL_SYSMON_MSG_PING_ID,
        .pMessagePayload = NULL,
        .payloadSize     = 0u,
        .pHandler        = svl_sysmonHandlePingRequest
    },
    [SVL_SYSMON_MSG_PING_RESP]              =
    {
        .messageId       = SVL_SYSMON_MSG_PING_RESP_ID,
        .pMessagePayload = (void_t*)&pingMessage,
        .payloadSize     = sizeof(pingMessage)
    },
    [SVL_SYSMON_MSG_CPU_USAGE_GET]          =
    {
        .messageId       = SVL_SYSMON_MSG_CPU_USAGE_GET_ID,
        .pMessagePayload = NULL,
        .payloadSize     = 0u,
        .pHandler        = svl_sysmonHandleCpuUsageGet
    },
    [SVL_SYSMON_MSG_CPU_USAGE_GET_RESP]     =
    {
        .messageId       = SVL_SYSMON_MSG_CPU_USAGE_GET_RESP_ID,
        .pMessagePayload = (void_t*)&cpuMessage,
        .payloadSize     = sizeof(cpuMessage)
    },
    [SVL_SYSMON_MSG_TASK_GET_DATA]          =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_GET_DATA_ID,
        .pMessagePayload = (void_t*)&taskDataGetMsg,
        .payloadSize     = sizeof(taskDataGetMsg),
        .pHandler        = svl_sysmonHandleTaskDataGet
    },
    [SVL_SYSMON_MSG_TASK_GET_DATA_RESP]     =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_GET_DATA_RESP_ID,
        .pMessagePayload = (void_t*)&taskDataMsg,
        .payloadSize     = sizeof(taskDataMsg)
    },
    [SVL_SYSMON_MSG_TASK_GET_VAR_DATA]      =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_GET_VAR_DATA_ID,
        .pMessagePayload = (void_t*)&taskDataGetMsg,
        .payloadSize     = sizeof(taskDataGetMsg),
        .pHandler        = svl_sysmonHandleTaskVariableDataGet
    },
    [SVL_SYSMON_MSG_TASK_GET_VAR_DATA_RESP] =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_GET_VAR_DATA_RESP_ID,
        .pMessagePayload = (void_t*)&taskVariableDataMsg,
        .payloadSize     = sizeof(taskVariableDataMsg)
    },
    [SVL_SYSMON_MSG_TASK_MODIFY_STATE]      =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_MODIFY_STATE_ID,
        .pMessagePayload = (void_t*)&taskModifyMessage,
        .payloadSize     = sizeof(taskModifyMessage),
        .pHandler        = svl_sysmonHandleTaskModification
    },
    [SVL_SYSMON_MSG_TASK_MODIFY_STATE_RESP] =
    {
        .messageId       = SVL_SYSMON_MSG_TASK_MODIFY_STATE_RESP_ID,
        .pMessagePayload = (void_t*)&taskModifyResultMessage,
        .payloadSize     = sizeof(taskModifyResultMessage)
    },
    [SVL_SYSMON_MSG_SYSRUNTIME_GET]         =
    {
        .messageId       = SVL_SYSMON_MSG_SYSRUNTIME_GET_ID,
        .pMessagePayload = NULL,
        .payloadSize     = 0u,
        .pHandler        = svl_sysmonHandleSysRuntimeGet
    },
    [SVL_SYSMON_MSG_SYSRUNTIME_GET_RESP]    =
    {
        .messageId       = SVL_SYSMON_MSG_SYSRUNTIME_GET_RESP_ID,
        .pMessagePayload = (void_t*)&sysRuntimeGetResultMessage,
        .payloadSize     = sizeof(sysRuntimeGetResultMessage)
    },
    [SVL_SYSMON_MSG_SYSTIME_SET]            =
    {
        .messageId       = SVL_SYSMON_MSG_SYSTIME_SET_ID,
        .pMessagePayload = (void_t*)&sysTimeSetMessage,
        .payloadSize     = sizeof(sysTimeSetMessage),
        .pHandler        = svl_sysmonHandleSystimeSet
    },
    [SVL_SYSMON_MSG_SYSTIME_SET_RESP]       =
    {
        .messageId       = SVL_SYSMON_MSG_SYSTIME_SET_RESP_ID,
        .pMessagePayload = (void_t*)&sysTimeSetResultMessage,
        .payloadSize     = sizeof(sysTimeSetResultMessage)
    },
    [SVL_SYSMON_MSG_RESET_REQ]              =
    {
        .messageId       = SVL_SYSMON_MSG_RESET_REQ_ID,
        .pMessagePayload = NULL,
        .payloadSize     = 0u,
        .pHandler        = svl_sysmonHandleResetRequest
    },
};

/*
 * Function: svl_sysmonInit
 */
gos_result_t svl_sysmonInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t sysmonInitResult = GOS_SUCCESS;
    u8_t         userMessageIndex = 0u;

    /*
     * Function code.
     */
    for (userMessageIndex = 0u; userMessageIndex < SVL_SYSMON_MAX_USER_MESSAGES; userMessageIndex++)
    {
        userMessages[userMessageIndex].callback    = NULL;
        userMessages[userMessageIndex].payload     = NULL;
        userMessages[userMessageIndex].payloadSize = 0u;
        userMessages[userMessageIndex].messageId   = SVL_SYSMON_MSG_INVALID_ID;
    }

    if (sysmonConfig.wiredRxFunction != NULL &&
    	sysmonConfig.wiredTxFunction != NULL)
    {
    	GOS_CONCAT_RESULT(sysmonInitResult, gos_gcpRegisterPhysicalDriver(sysmonConfig.wiredChannelNumber,
    			sysmonConfig.wiredTxFunction, sysmonConfig.wiredRxFunction));
        // Register task.
        GOS_CONCAT_RESULT(sysmonInitResult, gos_taskRegister(&sysmonWiredDaemonTaskDesc, NULL));
    }
    else
    {
    	// Wired service not configured.
    }

    if (sysmonConfig.wirelessRxFunction != NULL &&
		sysmonConfig.wirelessTxFunction != NULL)
    {
    	GOS_CONCAT_RESULT(sysmonInitResult, gos_gcpRegisterPhysicalDriver(sysmonConfig.wirelessChannelNumber,
    			sysmonConfig.wirelessTxFunction, sysmonConfig.wirelessRxFunction));
    	// Register task.
    	GOS_CONCAT_RESULT(sysmonInitResult, gos_taskRegister(&sysmonWirelessDaemonTaskDesc, NULL));
    }
    else
    {
    	// Wireless service not configured.
    }

    // Initialize sysmon mutex.
    GOS_CONCAT_RESULT(sysmonInitResult, gos_mutexInit(&sysmonMutex));

    return sysmonInitResult;
}

/*
 * Function: svl_sysmonRegisterUserMessage
 */
gos_result_t svl_sysmonRegisterUserMessage (svl_sysmonUserMessageDescriptor_t* pDesc)
{
    /*
     * Local variables.
     */
    gos_result_t registerResult   = GOS_ERROR;
    u8_t         userMessageIndex = 0u;

    /*
     * Function code.
     */
    if ((pDesc != NULL) && (pDesc->callback != NULL))
    {
        for (userMessageIndex = 0u; userMessageIndex < SVL_SYSMON_MAX_USER_MESSAGES; userMessageIndex++)
        {
            if (userMessages[userMessageIndex].messageId == SVL_SYSMON_MSG_INVALID_ID &&
                userMessages[userMessageIndex].callback  == NULL)
            {
                // Store user message descriptor.
                (void_t) memcpy((void_t*)&userMessages[userMessageIndex], (void_t*)pDesc, sizeof(*pDesc));
                registerResult = GOS_SUCCESS;
                break;
            }
            else
            {
            	// Continue searching.
            }
        }
    }
    else
    {
        // Error.
    }

    return registerResult;
}

/**
 * @brief   Fills the task data structure.
 * @details Copies the relevant fields of the task descriptor to the task
 *          data structure.
 *
 * @param[out] pSysmonTaskData Pointer to the task data structure to copy to.
 * @param[in]  pTaskData       Pointer to the task descriptor structure to copy from.
 *
 * @return  Result of data filling.
 * @retval  #GOS_SUCCESS Data filled successfully.
 * @retval  #GOS_ERROR   Either of the function parameters is NULL.
 */
GOS_STATIC gos_result_t svl_sysmonFillTaskData (svl_sysmonTaskData_t* pSysmonTaskData, gos_taskDescriptor_t* pTaskData)
{
	/*
	 * Local variables.
	 */
	gos_result_t fillResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pSysmonTaskData != NULL) && (pTaskData != NULL))
	{
	    pSysmonTaskData->taskId               = pTaskData->taskId;
	    pSysmonTaskData->taskStackSize        = pTaskData->taskStackSize;
	    pSysmonTaskData->taskStackMaxUsage    = pTaskData->taskStackSizeMaxUsage;
	    pSysmonTaskData->taskCpuUsageLimit    = pTaskData->taskCpuUsageLimit;
	    pSysmonTaskData->taskCpuUsage         = pTaskData->taskCpuUsage;
	    pSysmonTaskData->taskCpuUsageMax      = pTaskData->taskCpuUsageMax;
	    pSysmonTaskData->taskOriginalPriority = pTaskData->taskOriginalPriority;
	    pSysmonTaskData->taskPriority         = pTaskData->taskPriority;
	    pSysmonTaskData->taskCsCounter        = pTaskData->taskCsCounter;
	    pSysmonTaskData->taskPrivilegeLevel   = pTaskData->taskPrivilegeLevel;
	    pSysmonTaskData->taskState            = pTaskData->taskState;

	    (void_t) memcpy((void_t*)&(pSysmonTaskData->taskRunTime), (void_t*)&(pTaskData->taskRunTime), sizeof(pTaskData->taskRunTime));
	    (void_t) strcpy(pSysmonTaskData->taskName, pTaskData->taskName);

	    fillResult = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return fillResult;
}

/**
 * @brief   Fills the variable task data structure.
 * @details Copies the relevant fields of the task descriptor to the variable task
 *          data structure.
 *
 * @param[out] pSysmonTaskVariableData Pointer to the variable task data structure to copy to.
 * @param[in]  pTaskData               Pointer to the task descriptor structure to copy from.
 *
 * @return  Result of variable data filling.
 * @retval  #GOS_SUCCESS Data filled successfully.
 * @retval  #GOS_ERROR   Either of the function parameters is NULL.
 */
GOS_STATIC gos_result_t svl_sysmonFillTskVariableData (svl_sysmonTaskVariableData_t* pSysmonTaskVariableData, gos_taskDescriptor_t* pTaskData)
{
	/*
	 * Local variables.
	 */
	gos_result_t fillResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pSysmonTaskVariableData != NULL) && (pTaskData != NULL))
	{
        pSysmonTaskVariableData->taskStackMaxUsage = pTaskData->taskStackSizeMaxUsage;
        pSysmonTaskVariableData->taskCpuUsage      = pTaskData->taskCpuUsage;
        pSysmonTaskVariableData->taskCpuUsageMax   = pTaskData->taskCpuUsageMax;
        pSysmonTaskVariableData->taskPriority      = pTaskData->taskPriority;
        pSysmonTaskVariableData->taskCsCounter     = pTaskData->taskCsCounter;
        pSysmonTaskVariableData->taskState         = pTaskData->taskState;

        (void_t) memcpy((void_t*)&(pSysmonTaskVariableData->taskRunTime), (void_t*)&(pTaskData->taskRunTime), sizeof(pTaskData->taskRunTime));

	    fillResult = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return fillResult;
}

/**
 * @brief   Gets the LUT index of the given message.
 * @details Returns the look-up table index that belongs to the given message ID.
 *
 * @param[in] messageId ID of the message to get the index for.
 *
 * @return  Look-up table index of the message.
 */
GOS_STATIC svl_sysmonMessageEnum_t svl_sysmonGetLutIndex (svl_sysmonMessageId_t messageId)
{
    /*
     * Local variables.
     */
	svl_sysmonMessageEnum_t index = 0u;

    /*
     * Function code.
     */
    for (index = 0u; index < SVL_SYSMON_MSG_NUM_OF_MESSAGES; index++)
    {
        if (sysmonLut[index].messageId == messageId)
        {
            break;
        }
        else
        {
            // Continue searching.
        }
    }

    return index;
}

/**
 * @brief   Checks the high-level message parameters.
 * @details Checks the protocol version and the payload CRC value.
 *
 * @param[in] pBuffer  Pointer to the buffer to copy from.
 * @param[in] lutIndex Look-up table index of the message.
 *
 * @return  Result of message checking.
 *
 * @retval  #SVL_SYSMON_MSG_RES_OK          Message OK.
 */
GOS_STATIC svl_sysmonMessageResult_t svl_sysmonCheckMessage (void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Local variables.
     */
    svl_sysmonMessageResult_t result = SVL_SYSMON_MSG_RES_OK;

    /*
     * Function code.
     */
    // Check if there is a payload to be copied.
    if (sysmonLut[lutIndex].pMessagePayload != NULL)
    {
        (void_t) memcpy(sysmonLut[lutIndex].pMessagePayload, pBuffer, sysmonLut[lutIndex].payloadSize);
    }
    else
    {
        // Nothing to do.
    }

    return result;
}

/**
 * @brief   Sends the response to the given message.
 * @details Fills out the remaining transmit header parameters (except for the result field),
 *          and transmits the message with the corresponding payload.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonSendResponse (gos_gcpChannelNumber_t gcpChannel, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    (void_t) gos_gcpTransmitMessage(
    		gcpChannel,                          sysmonLut[lutIndex].messageId,
            sysmonLut[lutIndex].pMessagePayload, sysmonLut[lutIndex].payloadSize,
			0xFFFF
            );
}

/**
 * @brief   Handles the ping request.
 * @details Sends a ping response.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandlePingRequest (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    pingMessage.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);
    (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
}

/**
 * @brief   Handles the CPU usage get request.
 * @details Sends out the current CPU usage.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleCpuUsageGet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    cpuMessage.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (cpuMessage.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        cpuMessage.cpuUsage = gos_kernelGetCpuUsage();
    }
    else
    {
        // Message error.
    }

    (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
}

/**
 * @brief   Handles the task data get request.
 * @details Sends out the static task data for the requested task.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleTaskDataGet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Local variables.
     */
    u16_t  taskIndex = 0u;
    bool_t breakLoop = GOS_FALSE;

    /*
     * Function code.
     */
    taskDataMsg.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (taskDataMsg.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        // Task get data message OK.
        if (taskDataGetMsg.taskIndex == 0xFFFF)
        {
            // Send all task data.
            for (taskIndex = 0; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
            {
                if (breakLoop == GOS_TRUE)
                {
                    break;
                }
                else
                {
                    // Nothing to do, continue.
                }

                if (gos_taskGetDataByIndex(taskIndex, &taskDesc) == GOS_SUCCESS)
                {
                	(void_t) svl_sysmonFillTaskData(&taskDataMsg.taskData, &taskDesc);
                    taskDataMsg.messageResult = SVL_SYSMON_MSG_RES_OK;
                }
                else
                {
                    // Last task found.
                    taskDataMsg.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                    breakLoop = GOS_TRUE;
                }

                (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
            }
        }
        else
        {
            // Send specific task data.
            if (gos_taskGetDataByIndex(taskDataGetMsg.taskIndex, &taskDesc) == GOS_SUCCESS)
            {
                (void_t) svl_sysmonFillTaskData(&taskDataMsg.taskData, &taskDesc);
                taskDataMsg.messageResult = SVL_SYSMON_MSG_RES_OK;
            }
            else
            {
            	taskDataMsg.messageResult = SVL_SYSMON_MSG_RES_ERROR;
            }

            (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
        }
    }
    else
    {
        // Message error.
    }
}

/**
 * @brief   Handles the task variable data get request.
 * @details Sends out the variable task data related to the request task.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleTaskVariableDataGet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Local variables.
     */
    u16_t  taskIndex = 0u;
    bool_t breakLoop = GOS_FALSE;

    /*
     * Function code.
     */
    taskVariableDataMsg.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (taskVariableDataMsg.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        // Task get data message OK.
        if (taskDataGetMsg.taskIndex == 0xFFFF)
        {
            // Send all task data.
            for (taskIndex = 0; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
            {
                if (breakLoop == GOS_TRUE)
                {
                    break;
                }
                else
                {
                    // Nothing to do, continue.
                }

                if (gos_taskGetDataByIndex(taskIndex, &taskDesc) == GOS_SUCCESS)
                {
                	(void_t) svl_sysmonFillTskVariableData(&taskVariableDataMsg.taskVariableData, &taskDesc);
                    taskVariableDataMsg.messageResult = SVL_SYSMON_MSG_RES_OK;

                }
                else
                {
                    // Last task found.
                    taskVariableDataMsg.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                    breakLoop = GOS_TRUE;
                }

                (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
            }
        }
        else
        {
            // Send specific task data.
            if (gos_taskGetDataByIndex(taskDataGetMsg.taskIndex, &taskDesc) == GOS_SUCCESS)
            {
                (void_t) svl_sysmonFillTskVariableData(&taskVariableDataMsg.taskVariableData, &taskDesc);
                taskVariableDataMsg.messageResult = SVL_SYSMON_MSG_RES_OK;
            }
            else
            {
            	taskVariableDataMsg.messageResult = SVL_SYSMON_MSG_RES_ERROR;
            }

            (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
        }
    }
    else
    {
        // Message error.
    }
}

/**
 * @brief   Handles the task modification request.
 * @details Performs the requested kind of modification on the requested task.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleTaskModification (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    taskModifyResultMessage.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (taskModifyResultMessage.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        // Send specific task data.
        (void_t) gos_taskGetDataByIndex(taskModifyMessage.taskIndex, &taskDesc);

         // Perform request based on modification type.
         switch (taskModifyMessage.modificationType)
         {
             case SVL_SYSMON_TASK_MOD_TYPE_SUSPEND:
             {
                 if (gos_taskSuspend(taskDesc.taskId) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             case SVL_SYSMON_TASK_MOD_TYPE_RESUME:
             {
                 if (gos_taskResume(taskDesc.taskId) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             case SVL_SYSMON_TASK_MOD_TYPE_DELETE:
             {
                 if (gos_taskDelete(taskDesc.taskId) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             case SVL_SYSMON_TASK_MOD_TYPE_BLOCK:
             {
                 if (gos_taskBlock(taskDesc.taskId, taskModifyMessage.param) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             case SVL_SYSMON_TASK_MOD_TYPE_UNBLOCK:
             {
                 if (gos_taskUnblock(taskDesc.taskId) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             case SVL_SYSMON_TASK_MOD_TYPE_WAKEUP:
             {
                 if (gos_taskWakeup(taskDesc.taskId) == GOS_SUCCESS)
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
                 }
                 else
                 {
                     taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
                 }
                 break;
             }
             default:
             {
                 taskModifyResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
             }
         }
    }
    else
    {
        // Message error.
    }

    (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
}

/**
 * @brief   Handles the system runtime get request.
 * @details Sends out the total system runtime since startup.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleSysRuntimeGet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    sysRuntimeGetResultMessage.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (sysRuntimeGetResultMessage.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        if (gos_runTimeGet(&sysRuntimeGetResultMessage.sysRuntime) == GOS_SUCCESS)
        {
            sysRuntimeGetResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
        }
        else
        {
            sysRuntimeGetResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
        }
    }
    else
    {
        // Message error.
    }

    (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
}

/**
 * @brief   Handles the system time set request.
 * @details Sets the system time to the given value.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleSystimeSet (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    sysTimeSetResultMessage.messageResult = svl_sysmonCheckMessage(pBuffer, lutIndex);

    if (sysTimeSetResultMessage.messageResult == SVL_SYSMON_MSG_RES_OK)
    {
        if (gos_timeSet(&sysTimeSetMessage.desiredSystemTime) == GOS_SUCCESS)
        {
            sysTimeSetResultMessage.messageResult = SVL_SYSMON_MSG_RES_OK;
        }
        else
        {
            sysTimeSetResultMessage.messageResult = SVL_SYSMON_MSG_RES_ERROR;
        }
    }
    else
    {
        // Message error.
    }

    (void_t) svl_sysmonSendResponse(gcpChannel, (lutIndex + 1));
}

/**
 * @brief   Handles the system reset request.
 * @details Resets the system.
 *
 * @param[in] gcpChannel GCP channel number to transmit the response on.
 * @param[in] pBuffer    The buffer with the received bytes.
 * @param[in] lutIndex   Look-up table index of the message.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonHandleResetRequest (gos_gcpChannelNumber_t gcpChannel, void_t* pBuffer, svl_sysmonMessageEnum_t lutIndex)
{
    /*
     * Function code.
     */
    if (svl_sysmonCheckMessage(pBuffer, lutIndex) == SVL_SYSMON_MSG_RES_OK)
    {
        gos_kernelReset();
    }
    else
    {
        // Nothing to do.
    }
}

/**
 * @brief   System monitoring wired daemon task.
 * @details Serves the incoming system monitoring requests
 *          on the wired GCP channel.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonWiredDaemonTask (void_t)
{
    /*
     * Local variables.
     */
    svl_sysmonMessageEnum_t lutIndex         = 0u;
    u8_t                    userMessageIndex = 0u;
    u16_t                   messageId        = 0u;

    /*
     * Function code.
     */
    for (;;)
    {
        // Reset message ID.
        messageId = 0u;

        // Check if a message was received.
        if (gos_gcpReceiveMessage(sysmonConfig.wiredChannelNumber, &messageId, wiredRxBuffer, SVL_SYSMON_WIRED_RX_BUFF_SIZE, 0xFFFF) == GOS_SUCCESS)
        {
    		if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
    		{
                // Get LUT index.
                lutIndex = svl_sysmonGetLutIndex(messageId);
                (void_t) gos_mutexUnlock(&sysmonMutex);
    		}
    		else
    		{
    			// Mutex error.
    		}

            // Check user registered messages.
            if ((lutIndex == SVL_SYSMON_MSG_UNKNOWN) || (lutIndex == SVL_SYSMON_MSG_NUM_OF_MESSAGES))
            {
                for (userMessageIndex = 0u; userMessageIndex < SVL_SYSMON_MAX_USER_MESSAGES; userMessageIndex++)
                {
                    if (userMessages[userMessageIndex].messageId == messageId)
                    {
                    	if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
                    	{
                            // If payload is not NULL, copy it.
                            if (userMessages[userMessageIndex].payload != NULL)
                            {
                                (void_t) memcpy(userMessages[userMessageIndex].payload, (void_t*)wiredRxBuffer, userMessages[userMessageIndex].payloadSize);
                            }
                            else
                            {
                                // Message has no payload.
                            }

                            // Call callback function.
                            if (userMessages[userMessageIndex].callback != NULL)
                            {
#if SVL_SYSMON_TRACE_LEVEL > 0
                                (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wired] User message received with ID: 0x%04x\r\n", messageId);
#endif
                            	userMessages[userMessageIndex].callback(sysmonConfig.wiredChannelNumber);
                            }
                            else
                            {
                            	// NULL pointer.
                            }

                    		(void_t) gos_mutexUnlock(&sysmonMutex);
                    	}
                    	else
                    	{
                    		// Mutex error.
                    	}
                    }
                    else
                    {
                        // Message error.
                    }
                }
            }
            else
            {
            	if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
            	{
                	// Check if handler exists.
                    if (sysmonLut[lutIndex].pHandler != NULL)
                    {
#if SVL_SYSMON_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wired] Sysmon message received with ID: 0x%04x\r\n", messageId);
#endif
                    	sysmonLut[lutIndex].pHandler(sysmonConfig.wiredChannelNumber, wiredRxBuffer, lutIndex);
                    }
                    else
                    {
#if SVL_SYSMON_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wired] Unknown message received with ID: 0x%04x\r\n", messageId);
#endif
                        (void_t) svl_sysmonSendResponse(sysmonConfig.wiredChannelNumber, SVL_SYSMON_MSG_UNKNOWN);
                    }

            		(void_t) gos_mutexUnlock(&sysmonMutex);
            	}
            	else
            	{
            		// Mutex error.
            	}
            }
        }
        else
        {
            // Reception error.
            (void_t) gos_taskSleep(5);
        }
    }
}

/**
 * @brief   System monitoring wireless daemon task.
 * @details Serves the incoming system monitoring requests
 *          on the wireless GCP channel.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sysmonWirelessDaemonTask (void_t)
{
    /*
     * Local variables.
     */
    svl_sysmonMessageEnum_t lutIndex         = 0u;
    u8_t                    userMessageIndex = 0u;
    u16_t                   messageId        = 0u;

    /*
     * Function code.
     */
    (void_t) gos_taskSleep(3000);

    for (;;)
    {
        // Reset message ID.
        messageId = 0u;

        // Check if a message was received.
        if (gos_gcpReceiveMessage(sysmonConfig.wirelessChannelNumber, &messageId, wirelessRxBuffer, SVL_SYSMON_WIRELESS_RX_BUFF_SIZE, 0xFFFFu) == GOS_SUCCESS)
        {
    		if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
    		{
                // Get LUT index.
                lutIndex = svl_sysmonGetLutIndex(messageId);
                (void_t) gos_mutexUnlock(&sysmonMutex);
    		}
    		else
    		{
    			// Mutex error.
    		}

            // Check user registered messages.
            if ((lutIndex == SVL_SYSMON_MSG_UNKNOWN) || (lutIndex == SVL_SYSMON_MSG_NUM_OF_MESSAGES))
            {
                for (userMessageIndex = 0u; userMessageIndex < SVL_SYSMON_MAX_USER_MESSAGES; userMessageIndex++)
                {
                    if (userMessages[userMessageIndex].messageId == messageId)
                    {
                    	if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
                    	{
                            // If payload is not NULL, copy it.
                            if (userMessages[userMessageIndex].payload != NULL)
                            {
                                (void_t) memcpy(userMessages[userMessageIndex].payload, (void_t*)wirelessRxBuffer, userMessages[userMessageIndex].payloadSize);
                            }
                            else
                            {
                                // Message has no payload.
                            }

                            // Call callback function.
                            if (userMessages[userMessageIndex].callback != NULL)
                            {
#if SVL_SYSMON_TRACE_LEVEL > 0
                                (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wireless] User message received with ID: 0x%04x\r\n", messageId);
#endif
                            	userMessages[userMessageIndex].callback(sysmonConfig.wirelessChannelNumber);
                            }
                            else
                            {
                            	// NULL pointer.
                            }

                    		(void_t) gos_mutexUnlock(&sysmonMutex);
                    	}
                    	else
                    	{
                    		// Mutex error.
                    	}
                    }
                    else
                    {
                        // Message error.
                    }
                }
            }
            else
            {
            	if (gos_mutexLock(&sysmonMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
            	{
                	// Check if handler exists.
                    if (sysmonLut[lutIndex].pHandler != NULL)
                    {
#if SVL_SYSMON_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wireless] Sysmon message received with ID: 0x%04x\r\n", messageId);
#endif
                    	sysmonLut[lutIndex].pHandler(sysmonConfig.wirelessChannelNumber, wirelessRxBuffer, lutIndex);
                    }
                    else
                    {
#if SVL_SYSMON_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "[Wireless] Unknown message received with ID: 0x%04x\r\n", messageId);
#endif
                        (void_t) svl_sysmonSendResponse(sysmonConfig.wirelessChannelNumber, SVL_SYSMON_MSG_UNKNOWN);
                    }

            		(void_t) gos_mutexUnlock(&sysmonMutex);
            	}
            	else
            	{
            		// Mutex error.
            	}
            }
        }
        else
        {
            // Reception error.
            //(void_t) gos_taskSleep(10);
        }
    }
}
