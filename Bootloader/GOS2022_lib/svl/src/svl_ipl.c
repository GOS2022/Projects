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
//! @file       svl_ipl.c
//! @author     Ahmed Gazar
//! @date       2024-12-15
//! @version    1.0
//!
//! @brief      GOS2022 Library / Inter-Processor Link source.
//! @details    For a more detailed description of this driver, please refer to @ref svl_ipl.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-15    Ahmed Gazar     Initial version created.
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
/*
 * Includes
 */
#include <svl_ipl.h>
#include <string.h>
#include <svl_cfg.h>
#include <svl_pdh.h>
#include <drv_crc.h>

/*
 * Macros
 */
/**
 * Ping OK value.
 */
#define SVL_IPL_PING_OK      ( 40u )

/**
 * Maximum number of attempting to connect.
 */
#define SVL_IPL_MAX_ATTEMPTS ( 5u )

/*
 * Type definitions
 */
/**
 * IPL message header.
 */
typedef struct
{
    u32_t messageId;     //!< Message ID.
    u32_t messageLength; //!< Message length in bytes.
    u32_t reserved;      //!< Reserved for future use.
    u32_t messageCrc;    //!< Message CRC.
}svl_iplMsgHeader_t;

/**
 * IPL message IDs.
 */
typedef enum
{
    IPL_MSG_ID_DISCOVERY         = 0x01,  //!< Discovery request.
    IPL_MSG_ID_DISCOVERY_ACK     = 0xA01, //!< Discovery acknowledge.
    IPL_MSG_ID_CONFIG            = 0x11,  //!< Configuration request.
    IPL_MSG_ID_CONFIG_ACK        = 0xA11, //!< Configuration acknowledge.
    IPL_MSG_ID_CONNECT           = 0x21,  //!< Connect request.
    IPL_MSG_ID_CONNECT_ACK       = 0xA21, //!< Connect acknowledge.
    IPL_MSG_ID_CPU_LOAD          = 0x31,  //!< CPU load request.
    IPL_MSG_ID_CPU_LOAD_ACK      = 0xA31, //!< CPU load acknowledge.
	IPL_MSG_ID_RESET             = 0x41,  //!< Reset request.
    IPL_MSG_ID_PING              = 0x51,  //!< Ping request.
    IPL_MSG_ID_PING_ACK          = 0xA51, //!< Ping acknowledge.
	IPL_MSG_ID_TASK_NUM          = 0x61,  //!< Task number request.
	IPL_MSG_ID_TASK_NUM_ACK      = 0xA61, //!< Task number acknowledge.
	IPL_MSG_ID_TASK_DATA         = 0x71,  //!< Task data request.
	IPL_MSG_ID_TASK_DATA_ACK     = 0xA71, //!< Task data acknowledge.
	IPL_MSG_ID_TASK_VAR_DATA     = 0x81,  //!< Task variable data request.
	IPL_MSG_ID_TASK_VAR_DATA_ACK = 0xA81, //!< Task variable data acknowledge.
	IPL_MSG_ID_RUNTIME           = 0x91,  //!< Runtime request.
	IPL_MSG_ID_RUNTIME_ACK       = 0xA91, //!< Runtime acknowledge.
	IPL_MSG_ID_SWINFO            = 0xA1,  //!< Software info request.
	IPL_MSG_ID_SWINFO_ACK        = 0xAA1, //!< Software info acknowledge.
	IPL_MSG_ID_TASK_MODIFY       = 0xB1,  //!< Task modify request.
	IPL_MSG_ID_TASK_MODIFY_ACK   = 0xAB1, //!< Task modify acknowledge.
    IPL_MSG_ID_HWINFO            = 0xC1,  //!< Hardware info request.
    IPL_MSG_ID_HWINFO_ACK        = 0xAC1, //!< Hardware info acknowledge.
    IPL_MSG_ID_SYNC_TIME         = 0xD1,  //!< System time synchronization request.
	IPL_MSG_ID_SYNC_TIME_ACK     = 0xAD1, //!< System time synchronization acknowledge.
}svl_iplMsgId_t;

/**
 * IPL task modification type.
 */
typedef enum
{
	IPL_TASK_MODIFY_SUSPEND = 0, //!< Suspend.
	IPL_TASK_MODIFY_RESUME  = 1, //!< Resume.
	IPL_TASK_MODIFY_DELETE  = 2, //!< Delete.
	IPL_TASK_MODIFY_BLOCK   = 3, //!< Block.
	IPL_TASK_MODIFY_UNBLOCK = 4, //!< Unblock.
	IPL_TASK_MODIFY_WAKEUP  = 5  //!< Wake up.
}svl_iplTaskModifyType_t;

/**
 * IPL ping message.
 */
typedef struct
{
    u8_t pingResult;
}svl_iplPingMsg_t;

/**
 * IPL system runtime message.
 */
typedef struct
{
	gos_runtime_t sysRunTime;
}svl_iplRunTimeMsg_t;

/**
 * IPL CPU load message.
 */
typedef struct
{
	u16_t cpuLoad;
}svl_iplCpuLoadMsg_t;

/**
 * IPL task number message.
 */
typedef struct
{
	u16_t taskNumber;
}svl_iplTaskNumberMsg_t;

/**
 * IPL task data get message.
 */
typedef struct
{
	u16_t taskIdx;
}svl_iplTaskDataGetMsg_t;

/**
 * IPL task modify message type.
 */
typedef struct
{
	u16_t taskIdx;
	u8_t  modificationType;
	u8_t  result;
}svl_iplTaskModifyMsg_t;

/**
 * IPL task variable data message.
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
}svl_iplTaskVariableDataMsg_t;

/**
 * IPL task data message.
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
}svl_iplTaskDataMsg_t;

/**
 * IPL system time set message.
 */
typedef struct __attribute__((packed))
{
    gos_time_t desiredSystemTime;                        //!< Desired system time.
}svl_iplSystimeSetMessage_t;

/**
 * IPL discovery message.
 */
typedef struct __attribute__((packed))
{
	char_t masterDeviceId [32];
	char_t slaveDeviceId [32];
}svl_iplDiscoveryMessage_t;

/**
 * IPL connect message.
 */
typedef struct
{
	bool_t connectSuccess;
}svl_iplConnectMessage_t;

/**
 * IPL state machine states.
 */
typedef enum
{
	SVL_IPL_STATE_NOT_CONFIGURED, //!< Driver functions are missing.
	SVL_IPL_STATE_DISCOVER_START, //!< Discovery start.
	SVL_IPL_STATE_DISCOVER,       //!< Discovery (device detection).
	SVL_IPL_STATE_CONFIG_START,   //!< Configuration start.
	SVL_IPL_STATE_CONFIG,         //!< Configuration.
	SVL_IPL_STATE_CONNECT_START,  //!< Connection start.
	SVL_IPL_STATE_CONNECT,        //!< Connection (to the configured network).
	SVL_IPL_STATE_CONNECTED       //!< Connected.
}svl_iplState_t;

/*
 * Static variables
 */
/**
 * IPL state machine variable.
 */
GOS_STATIC svl_iplState_t               iplState = SVL_IPL_STATE_NOT_CONFIGURED;

/**
 * IPL RX buffer.
 */
GOS_STATIC u8_t                         iplRxBuffer [SVL_IPL_RX_BUFF_SIZE];

/**
 * Task data.
 */
GOS_STATIC gos_taskDescriptor_t         taskData;

/**
 * Task data get message.
 */
GOS_STATIC svl_iplTaskDataGetMsg_t      taskDataGetMsg;

/**
 * Task variable data get message.
 */
GOS_STATIC svl_iplTaskVariableDataMsg_t taskVarDataMsg;

/**
 * Run-time message.
 */
GOS_STATIC svl_iplRunTimeMsg_t          runTimeMsg;

/**
 * Task data message.
 */
GOS_STATIC svl_iplTaskDataMsg_t         taskDataMsg;

/**
 * CPU load message.
 */
GOS_STATIC svl_iplCpuLoadMsg_t          cpuLoadMsg;

/**
 * Message header.
 */
GOS_STATIC svl_iplMsgHeader_t           msgHeader;

/**
 * Connect message.
 */
GOS_STATIC svl_iplConnectMessage_t      connectMsg;

/**
 * Connect result message.
 */
GOS_STATIC svl_iplConnectMessage_t      connectResultMsg;

/**
 * Ping message.
 */
GOS_STATIC svl_iplPingMsg_t             pingMsg;

/**
 * Task number message.
 */
GOS_STATIC svl_iplTaskNumberMsg_t       taskNumMsg;

/**
 * Device configuration message.
 */
GOS_STATIC svl_pdhWifiCfg_t             devConfigMsg;

/**
 * Received device configuration message.
 */
GOS_STATIC svl_pdhWifiCfg_t             receivedDevConfigMsg;

/**
 * Discovery message.
 */
GOS_STATIC svl_iplDiscoveryMessage_t    discoveryMsg;

/**
 * Hardware info message.
 */
GOS_STATIC svl_pdhHwInfo_t              hwInfoMsg;

/**
 * Software info message.
 */
GOS_STATIC svl_pdhSwInfo_t              swInfoMsg;

/**
 * Task modify message.
 */
GOS_STATIC svl_iplTaskModifyMsg_t       taskModifyMsg;

/**
 * System time set message.
 */
GOS_STATIC svl_iplSystimeSetMessage_t   sysTimeSetMsg;

/**
 * User-defined IPL messages.
 */
GOS_STATIC svl_iplUserMsgDesc_t         userMsgLut [SVL_IPL_USER_MSG_CALLBACK_MAX_NUM];

/**
 * IPL transmit function (user-defined).
 */
GOS_STATIC svl_iplTransmitFunction      svl_iplTransmit;

/**
 * IPL receive function (user-defined).
 */
GOS_STATIC svl_iplReceiveFunction       svl_iplReceive;

/**
 * IPL connection attempt counter.
 */
GOS_STATIC u8_t                         attemptCount = 0u;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_iplDaemon (void_t);

/**
 * IPL daemon descriptor.
 */
GOS_STATIC gos_taskDescriptor_t svlIplTaskDesc =
{
    .taskFunction 	    = svl_iplDaemon,
    .taskName 		    = "svl_ipl_daemon",
    .taskStackSize 	    = SVL_IPL_DAEMON_STACK_SIZE,
    .taskPriority 	    = SVL_IPL_DAEMON_PRIORITY,
    .taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: svl_iplInit
 */
gos_result_t svl_iplInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_ERROR;

    /*
     * Function code.
     */
    initResult = gos_taskRegister(&svlIplTaskDesc, NULL);

    return initResult;
}

/*
 * Function: svl_iplRegisterUserMsg
 */
gos_result_t svl_iplRegisterUserMsg (svl_iplUserMsgDesc_t* pUserMsgDesc)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;
	u8_t         lutIndex       = 0u;

	/*
	 * Function code.
	 */
	if (pUserMsgDesc != NULL)
	{
		for (lutIndex = 0u; lutIndex < SVL_IPL_USER_MSG_CALLBACK_MAX_NUM; lutIndex++)
		{
			if (userMsgLut[lutIndex].callback == NULL)
			{
				(void_t) memcpy((void_t*)&userMsgLut[lutIndex], (void_t*)pUserMsgDesc, sizeof(*pUserMsgDesc));
				registerResult = GOS_SUCCESS;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// NULL pointer.
	}

	return registerResult;
}

/*
 * Function: svl_iplConfigure
 */
gos_result_t svl_iplConfigure (svl_iplTransmitFunction transmitFunc, svl_iplReceiveFunction receiveFunc)
{
	/*
	 * Local variables.
	 */
	gos_result_t configureResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	svl_iplTransmit = transmitFunc;
	svl_iplReceive  = receiveFunc;

	// Set IPL state machine based on NULL pointer check.
	if (svl_iplTransmit != NULL && svl_iplReceive != NULL)
	{
		iplState = SVL_IPL_STATE_DISCOVER_START;
	}
	else
	{
		iplState = SVL_IPL_STATE_NOT_CONFIGURED;
	}

	return configureResult;
}

/*
 * Function: svl_iplSendMessage
 */
gos_result_t svl_iplSendMessage (u32_t msgId, u8_t* pData, u32_t dataLength)
{
	/*
	 * Local variables.
	 */
	gos_result_t       sendResult = GOS_ERROR;
	svl_iplMsgHeader_t header;

	/*
	 * Function code.
	 */
	header.messageId     = msgId;
	header.messageLength = dataLength;

	if (drv_crcGetCrc32((u8_t*)pData, dataLength, &header.messageCrc) == GOS_SUCCESS &&
		svl_iplTransmit((u8_t*)&header, sizeof(svl_iplMsgHeader_t), 1000u) == GOS_SUCCESS &&
		svl_iplTransmit(pData, dataLength, 1000u) == GOS_SUCCESS)
	{
		sendResult = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return sendResult;
}

/**
 * @brief   IPL daemon task.
 * @details Handles IPL state machine and incoming IPL messages.
 *
 * @return  -
 */
GOS_STATIC void_t svl_iplDaemon (void_t)
{
	/*
	 * Local variables.
	 */
	u8_t lutIndex = 0u;

	/*
	 * Function code.
	 */
	// Delay service start.
	(void_t) gos_taskSleep(2000);

	for (;;)
	{
		switch (iplState)
		{
			case SVL_IPL_STATE_NOT_CONFIGURED:
			{
				// IPL functions are not configured.
				// Wait until transmit and receive functions are registered.
				break;
			}
			case SVL_IPL_STATE_DISCOVER_START:
			{
				attemptCount++;

				if (attemptCount <= SVL_IPL_MAX_ATTEMPTS)
				{
#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTrace(GOS_TRUE, "IPL discovery start...\r\n");
#endif
					iplState = SVL_IPL_STATE_DISCOVER;
				}
				else
				{
					// No more attempts. Suspend task.
					(void_t) gos_taskSuspend(svlIplTaskDesc.taskId);

					// If task gets resumed externally, restart state-machine.
					attemptCount = 0u;
				}
				break;
			}
			case SVL_IPL_STATE_DISCOVER:
			{
				msgHeader.messageId     = IPL_MSG_ID_DISCOVERY;
				msgHeader.messageLength = sizeof(discoveryMsg);

				(void_t) strcpy(discoveryMsg.masterDeviceId, "STM32F446-0001");
				(void_t) drv_crcGetCrc32((u8_t*)&discoveryMsg, sizeof(discoveryMsg), &msgHeader.messageCrc);

				if (svl_iplTransmit((u8_t*)&msgHeader,    sizeof(svl_iplMsgHeader_t), 500u) == GOS_SUCCESS &&
					svl_iplTransmit((u8_t*)&discoveryMsg, sizeof(discoveryMsg),       500u) == GOS_SUCCESS &&
					svl_iplReceive((u8_t*)&msgHeader,     sizeof(msgHeader),          500u) == GOS_SUCCESS &&
					svl_iplReceive(iplRxBuffer,           msgHeader.messageLength,    500u) == GOS_SUCCESS &&
					msgHeader.messageId == IPL_MSG_ID_DISCOVERY_ACK &&
					drv_crcCheckCrc32(iplRxBuffer, msgHeader.messageLength, msgHeader.messageCrc, NULL) == DRV_CRC_CHECK_OK)
				{
                    // Convert buffer to received message.
                    (void_t) memcpy((void_t*)&discoveryMsg, (void_t*)iplRxBuffer, msgHeader.messageLength);
#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTraceFormatted(GOS_TRUE, "IPL device discovered: %s\r\n", discoveryMsg.slaveDeviceId);
#endif
					iplState = SVL_IPL_STATE_CONFIG_START;
				}
				else
				{
#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTrace(GOS_TRUE, "IPL discovery failed.\r\n");
#endif
					iplState = SVL_IPL_STATE_DISCOVER_START;
					(void_t) gos_taskSleep(5000);
				}
				break;
			}
			case SVL_IPL_STATE_CONFIG_START:
			{
#if SVL_IPL_TRACE_LEVEL > 0
				(void_t) gos_traceTrace(GOS_TRUE, "Configuring device...\r\n");
#endif
				iplState = SVL_IPL_STATE_CONFIG;
				break;
			}
			case SVL_IPL_STATE_CONFIG:
			{
				(void_t) svl_pdhGetWifiCfg(&devConfigMsg);

				msgHeader.messageId     = IPL_MSG_ID_CONFIG;
				msgHeader.messageLength = sizeof(devConfigMsg);

				(void_t) drv_crcGetCrc32((u8_t*)&devConfigMsg, sizeof(devConfigMsg), &msgHeader.messageCrc);

				if (svl_iplTransmit((u8_t*)&msgHeader,    sizeof(svl_iplMsgHeader_t), 1000u) == GOS_SUCCESS &&
					svl_iplTransmit((u8_t*)&devConfigMsg, sizeof(devConfigMsg),       1000u) == GOS_SUCCESS &&
					svl_iplReceive((u8_t*)&msgHeader,     sizeof(msgHeader),          2000u) == GOS_SUCCESS &&
					svl_iplReceive(iplRxBuffer,           msgHeader.messageLength,    2000u) == GOS_SUCCESS &&
					msgHeader.messageId == IPL_MSG_ID_CONFIG_ACK &&
					drv_crcCheckCrc32(iplRxBuffer, msgHeader.messageLength, msgHeader.messageCrc, NULL) == DRV_CRC_CHECK_OK)
				{
                    // Convert buffer to received message.
                    (void_t) memcpy((void_t*)&receivedDevConfigMsg, (void_t*)iplRxBuffer, msgHeader.messageLength);

#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTraceFormatted(
							GOS_TRUE,
							"\r\n"
							"IPL device configuration set to:\r\n"
							"SSID:      \t%s\r\n"
							"Password:  \t%s\r\n"
							"IP address:\t%u.%u.%u.%u\r\n"
							"Port:      \t%u\r\n"
							"Subnet:    \t%u.%u.%u.%u\r\n"
							"Gateway:   \t%u.%u.%u.%u\r\n",
							receivedDevConfigMsg.ssid,
							receivedDevConfigMsg.pwd,
							receivedDevConfigMsg.ipAddress[0], receivedDevConfigMsg.ipAddress[1],
							receivedDevConfigMsg.ipAddress[2], receivedDevConfigMsg.ipAddress[3],
							receivedDevConfigMsg.port,
							receivedDevConfigMsg.subnet[0],  receivedDevConfigMsg.subnet[1],
							receivedDevConfigMsg.subnet[2],  receivedDevConfigMsg.subnet[3],
							receivedDevConfigMsg.gateway[0], receivedDevConfigMsg.gateway[1],
							receivedDevConfigMsg.gateway[2], receivedDevConfigMsg.gateway[3]
						);
#endif
					iplState = SVL_IPL_STATE_CONNECT_START;
				}
				else
				{
#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTrace(GOS_TRUE, "IPL configuration failed.\r\n");
#endif
					iplState = SVL_IPL_STATE_DISCOVER_START;
					(void_t) gos_taskSleep(1000);
				}
				break;
			}
			case SVL_IPL_STATE_CONNECT_START:
			{
#if SVL_IPL_TRACE_LEVEL > 0
				(void_t) gos_traceTrace(GOS_TRUE, "Connecting to network...\r\n");
#endif
				iplState = SVL_IPL_STATE_CONNECT;
				break;
			}
			case SVL_IPL_STATE_CONNECT:
			{
				msgHeader.messageId     = IPL_MSG_ID_CONNECT;
				msgHeader.messageLength = sizeof(connectMsg);

				(void_t) drv_crcGetCrc32((u8_t*)&connectMsg, sizeof(connectMsg), &msgHeader.messageCrc);

				if (svl_iplTransmit((u8_t*)&msgHeader,  sizeof(svl_iplMsgHeader_t), 1000u) == GOS_SUCCESS &&
					svl_iplTransmit((u8_t*)&connectMsg, sizeof(connectMsg),         1000u) == GOS_SUCCESS &&
					svl_iplReceive((u8_t*)&msgHeader,   sizeof(msgHeader),         10000u) == GOS_SUCCESS &&
					svl_iplReceive(iplRxBuffer,         msgHeader.messageLength,    2000u) == GOS_SUCCESS &&
					msgHeader.messageId == IPL_MSG_ID_CONNECT_ACK &&
					drv_crcCheckCrc32(iplRxBuffer, msgHeader.messageLength, msgHeader.messageCrc, NULL) == DRV_CRC_CHECK_OK)
				{
                    // Convert buffer to received message.
                    (void_t) memcpy((void_t*)&connectResultMsg, (void_t*)iplRxBuffer, msgHeader.messageLength);

					if (connectResultMsg.connectSuccess == GOS_TRUE)
					{
#if SVL_IPL_TRACE_LEVEL > 0
						(void_t) gos_traceTrace(GOS_TRUE, "Successfully connected to the network.\r\n");
#endif
						iplState = SVL_IPL_STATE_CONNECTED;
					}
					else
					{
#if SVL_IPL_TRACE_LEVEL > 0
						(void_t) gos_traceTrace(GOS_TRUE, "Network connection failed.\r\n");
#endif
						iplState = SVL_IPL_STATE_DISCOVER_START;
					}
				}
				else
				{
					// No response.
#if SVL_IPL_TRACE_LEVEL > 0
					(void_t) gos_traceTrace(GOS_TRUE, "No response was received.\r\n");
#endif
					iplState = SVL_IPL_STATE_DISCOVER_START;
				}

				break;
			}
			case SVL_IPL_STATE_CONNECTED:
			{
				// Check if there is an incoming request.
				if (svl_iplReceive((u8_t*)&msgHeader, sizeof(msgHeader),       0xFFFFFFFFu) == GOS_SUCCESS &&
					svl_iplReceive(iplRxBuffer,       msgHeader.messageLength, 0xFFFFFFFFu) == GOS_SUCCESS)
				{
					if (drv_crcCheckCrc32(iplRxBuffer, msgHeader.messageLength, msgHeader.messageCrc, NULL) == DRV_CRC_CHECK_OK)
					{
						switch (msgHeader.messageId)
						{
							case IPL_MSG_ID_CPU_LOAD:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL CPU load request received.\r\n");
#endif
								cpuLoadMsg.cpuLoad = gos_kernelGetCpuUsage();
								(void_t) svl_iplSendMessage(IPL_MSG_ID_CPU_LOAD_ACK, (u8_t*)&cpuLoadMsg, sizeof(cpuLoadMsg));
								break;
							}
							case IPL_MSG_ID_RESET:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL reset request received.\r\n");
#endif
								(void_t) gos_taskSleep(2000);
								gos_kernelReset();
								break;
							}
							case IPL_MSG_ID_PING:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL ping request received.\r\n");
#endif
								pingMsg.pingResult = SVL_IPL_PING_OK;
								(void_t) svl_iplSendMessage(IPL_MSG_ID_PING_ACK, (u8_t*)&pingMsg, sizeof(pingMsg));
								break;
							}
							case IPL_MSG_ID_TASK_NUM:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL task number request received.\r\n");
#endif
								(void_t) gos_taskGetNumber(&taskNumMsg.taskNumber);
								(void_t) svl_iplSendMessage(IPL_MSG_ID_TASK_NUM_ACK, (u8_t*)&taskNumMsg, sizeof(taskNumMsg));
								break;
							}
							case IPL_MSG_ID_TASK_DATA:
							{
								// Get task index.
								(void_t) memcpy((void_t*)&taskDataGetMsg, (void_t*)iplRxBuffer, sizeof(taskDataGetMsg));
								// Get task data.
								(void_t) gos_taskGetDataByIndex(taskDataGetMsg.taskIdx, &taskData);
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTraceFormatted(
										GOS_TRUE,
										"IPL task data request received. Idx: %u.\r\n",
										taskDataGetMsg.taskIdx);
#endif
								// Fill out message.
								taskDataMsg.taskCpuUsage         = taskData.taskCpuUsage;
								taskDataMsg.taskCpuUsageLimit    = taskData.taskCpuUsageLimit;
								taskDataMsg.taskCpuUsageMax      = taskData.taskCpuUsageMax;
								taskDataMsg.taskCsCounter        = taskData.taskCsCounter;
								taskDataMsg.taskId               = taskData.taskId;
								(void_t) strcpy(taskDataMsg.taskName, taskData.taskName);
								taskDataMsg.taskOriginalPriority = taskData.taskOriginalPriority;
								taskDataMsg.taskPriority         = taskData.taskPriority;
								taskDataMsg.taskPrivilegeLevel   = taskData.taskPrivilegeLevel;
								(void_t) memcpy(&taskDataMsg.taskRunTime, &taskData.taskRunTime, sizeof(taskData.taskRunTime));
								taskDataMsg.taskStackMaxUsage    = taskData.taskStackSizeMaxUsage;
								taskDataMsg.taskStackSize        = taskData.taskStackSize;
								taskDataMsg.taskState            = taskData.taskState;

								(void_t) svl_iplSendMessage(IPL_MSG_ID_TASK_DATA_ACK, (u8_t*)&taskDataMsg, sizeof(taskDataMsg));
								break;
							}
							case IPL_MSG_ID_TASK_VAR_DATA:
							{
								// Get task index.
								(void_t) memcpy((void_t*)&taskDataGetMsg, (void_t*)iplRxBuffer, sizeof(taskDataGetMsg));
								// Get task data.
								(void_t) gos_taskGetDataByIndex(taskDataGetMsg.taskIdx, &taskData);
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTraceFormatted(
										GOS_TRUE,
										"IPL task variable data request received. Idx: %u.\r\n",
										taskDataGetMsg.taskIdx);
#endif
								taskVarDataMsg.taskStackMaxUsage = taskData.taskStackSizeMaxUsage;
								taskVarDataMsg.taskCpuUsage      = taskData.taskCpuUsage;
								taskVarDataMsg.taskCpuUsageMax   = taskData.taskCpuUsageMax;
								taskVarDataMsg.taskPriority      = taskData.taskPriority;
								taskVarDataMsg.taskCsCounter     = taskData.taskCsCounter;
			                    taskVarDataMsg.taskState         = taskData.taskState;

			                    (void_t) memcpy((void_t*)&taskVarDataMsg.taskRunTime, (void_t*)&taskData.taskRunTime, sizeof(taskData.taskRunTime));

								(void_t) svl_iplSendMessage(IPL_MSG_ID_TASK_VAR_DATA_ACK, (u8_t*)&taskVarDataMsg, sizeof(taskVarDataMsg));

								break;
							}
							case IPL_MSG_ID_RUNTIME:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL runtime request received.\r\n");
#endif
								(void_t) gos_runTimeGet(&runTimeMsg.sysRunTime);
								(void_t) svl_iplSendMessage(IPL_MSG_ID_RUNTIME_ACK, (u8_t*)&runTimeMsg, sizeof(runTimeMsg));
								break;
							}
							case IPL_MSG_ID_HWINFO:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL hardware info request received.\r\n");
#endif
								(void_t) svl_pdhGetHwInfo(&hwInfoMsg);
								(void_t) svl_iplSendMessage(IPL_MSG_ID_HWINFO_ACK, (u8_t*)&hwInfoMsg, sizeof(hwInfoMsg));
								break;
							}
							case IPL_MSG_ID_SWINFO:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL software info request received.\r\n");
#endif
								(void_t) svl_pdhGetSwInfo(&swInfoMsg);
								(void_t) svl_iplSendMessage(IPL_MSG_ID_SWINFO_ACK, (u8_t*)&swInfoMsg, sizeof(swInfoMsg));
								break;
							}
							case IPL_MSG_ID_SYNC_TIME:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTrace(GOS_TRUE, "IPL system time synchronization request received.\r\n");
#endif
								(void_t) memcpy((void_t*)&sysTimeSetMsg, (void_t*)iplRxBuffer, sizeof(sysTimeSetMsg));
								(void_t) gos_timeSet(&sysTimeSetMsg.desiredSystemTime);
								(void_t) svl_iplSendMessage(IPL_MSG_ID_SYNC_TIME_ACK, (u8_t*)&sysTimeSetMsg, sizeof(sysTimeSetMsg));
								break;
							}
							case IPL_MSG_ID_TASK_MODIFY:
							{
								// Get parameters.
								(void_t) memcpy((void_t*)&taskModifyMsg, (void_t*)iplRxBuffer, sizeof(taskModifyMsg));
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTraceFormatted(
										GOS_TRUE,
										"IPL task modification request received. Idx: %u Type: %u.\r\n",
										taskModifyMsg.taskIdx,
										taskModifyMsg.modificationType);
#endif

								(void_t) gos_taskGetDataByIndex(taskModifyMsg.taskIdx, &taskData);

								switch (taskModifyMsg.modificationType)
								{
									case IPL_TASK_MODIFY_SUSPEND:
									{
										taskModifyMsg.result = gos_taskSuspend(taskData.taskId) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									case IPL_TASK_MODIFY_RESUME:
									{
										taskModifyMsg.result = gos_taskResume(taskData.taskId) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									case IPL_TASK_MODIFY_DELETE:
									{
										taskModifyMsg.result = gos_taskDelete(taskData.taskId) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									case IPL_TASK_MODIFY_BLOCK:
									{
										taskModifyMsg.result = gos_taskBlock(taskData.taskId, GOS_TASK_MAX_BLOCK_TIME_MS) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									case IPL_TASK_MODIFY_UNBLOCK:
									{
										taskModifyMsg.result = gos_taskUnblock(taskData.taskId) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									case IPL_TASK_MODIFY_WAKEUP:
									{
										taskModifyMsg.result = gos_taskWakeup(taskData.taskId) == GOS_SUCCESS ? 1 : 0;
										break;
									}
									default:
									{
										taskModifyMsg.result = 0;
										break;
									}
								}

								(void_t) svl_iplSendMessage(IPL_MSG_ID_TASK_MODIFY_ACK, (u8_t*)&taskModifyMsg, sizeof(taskModifyMsg));
								break;
							}
							default:
							{
#if SVL_IPL_TRACE_LEVEL == 2
								(void_t) gos_traceTraceFormatted(GOS_TRUE, "IPL request received with ID: %u.\r\n", msgHeader.messageId);
#endif
								for (lutIndex = 0u; lutIndex < SVL_IPL_USER_MSG_CALLBACK_MAX_NUM; lutIndex++)
								{
									if (userMsgLut[lutIndex].callback == NULL)
									{
										// Last registered message found, break loop.
										break;
									}
									else
									{
										if (userMsgLut[lutIndex].msgId == msgHeader.messageId)
										{
											userMsgLut[lutIndex].callback(iplRxBuffer, msgHeader.messageLength, msgHeader.messageCrc);
										}
										else
										{
											// Continue.
										}
									}
								}
							}
						}
					}
					else
					{
						(void_t) gos_taskSleep(10);
					}
				}
				break;
			}
		}
		(void_t) gos_taskSleep(10);
	}
}
