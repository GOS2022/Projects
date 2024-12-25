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
//! @file       svl_sdh.c
//! @author     Ahmed Gazar
//! @date       2024-12-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / Software Download Handler
//! @details    For a more detailed description of this driver, please refer to @ref svl_sdh.h
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
/*
 * Includes
 */
#include <svl_cfg.h>
#include <svl_ipl.h>
#include <svl_sdh.h>
#include <string.h>

/*
 * Macros
 */
#define SVL_SDH_STORAGE_DESC_AREA_START     ( 0u    )
#define SVL_SDH_STORAGE_DESC_BIN_DESC_START ( SVL_SDH_STORAGE_DESC_AREA_START + sizeof(u16_t) )
#define SVL_SDH_STORAGE_DESC_AREA_SIZE      ( 4096u )
#define SVL_SDH_STORAGE_DESC_AREA_END       ( SVL_SDH_STORAGE_DESC_AREA_START + SVL_SDH_STORAGE_DESC_AREA_SIZE - 1 )
#define SVL_SDH_BINARY_AREA_START           ( SVL_SDH_STORAGE_DESC_AREA_END + 1 )
#define SVL_SDH_BINARY_AREA_SIZE            ( 8388608 - SVL_SDH_BINARY_AREA_START )
#define SVL_SDH_BINARY_AREA_END             ( SVL_SDH_BINARY_AREA_START + SVL_SDH_BINARY_AREA_SIZE - 1 )
#define SVL_SDH_STATE_CONT_MSG_ID           ( 0xa3ed )
#define SVL_SDH_STATE_CONT_RESP_MSG_ID      ( 0xa3ef )
#define SVL_SDH_CHUNK_SIZE                  ( 2048u )
#define SVL_SDH_BUFFER_SIZE                 ( SVL_SDH_CHUNK_SIZE + 16u )

/*
 * Type definitions
 */
// TODO
typedef enum
{
	SDH_STATE_IDLE,
	SDH_STATE_DOWNLOADING_BINARY,

	SDH_STATE_BINARY_NUM_REQ,
	SDH_STATE_BINARY_INFO_REQ,
	SDH_STATE_BINARY_DOWNLOAD_REQ,
}svl_sdhState_t;

typedef enum
{
	SDH_DOWNLOAD_REQ_OK = 1,
	SDH_DOWNLOAD_REQ_DESC_SIZE_ERR = 2,
	SDH_DOWNLOAD_REQ_FILE_SIZE_ERR = 4
}svl_sdhDownloadReqRes_t;

// TODO
typedef struct
{
	svl_sdhState_t requiredState;
	u8_t*          pData;
	u16_t          dataSize;
}svl_sdhControlMsg_t;

typedef struct __attribute__((packed))
{
	u16_t chunkIdx;
	u8_t  result;
}svl_sdhChunkDesc_t;

/*
 * Static variables
 */
/**
 * Read function pointer.
 */
GOS_STATIC svl_sdhReadWriteFunc_t sdhReadFunction = NULL;

/**
 * Write function pointer.
 */
GOS_STATIC svl_sdhReadWriteFunc_t sdhWriteFunction = NULL;

// TODO
GOS_STATIC u8_t  sdhBuffer [SVL_SDH_BUFFER_SIZE];

// TODO
GOS_STATIC gos_message_t sdhStateControlMsg =
{
	.messageId   = SVL_SDH_STATE_CONT_MSG_ID,
	.messageSize = sizeof(svl_sdhControlMsg_t)
};

// TODO
GOS_STATIC svl_sdhState_t sdhState = SDH_STATE_IDLE;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_sdhDaemon (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryNumReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryInfoReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonDownloadReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryChunkReqCallback (void_t);

GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback (u8_t* pData, u32_t size, u32_t crc);

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryNumReqMsg =
{
	.callback        = svl_sdhSysmonBinaryNumReqCallback,
	.messageId       = 0x2101,
	.payload         = NULL,
	.payloadSize     = 0u,
	.protocolVersion = 1u
};

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryInfoReqMsg =
{
	.callback        = svl_sdhSysmonBinaryInfoReqCallback,
	.messageId       = 0x2102,
	.payload         = (void_t*)sdhBuffer,
	.payloadSize     = sizeof(u16_t),
	.protocolVersion = 1u
};

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonDownloadReqMsg =
{
	.callback        = svl_sdhSysmonDownloadReqCallback,
	.messageId       = 0x2103,
	.payload         = (void_t*)sdhBuffer,
	.payloadSize     = sizeof(svl_sdhBinaryDesc_t),
	.protocolVersion = 1u
};

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryChunkReqMsg =
{
	.callback        = svl_sdhSysmonBinaryChunkReqCallback,
	.messageId       = 0x2104,
	.payload         = (void_t*)sdhBuffer,
	.payloadSize     = sizeof(svl_sdhChunkDesc_t) + SVL_SDH_CHUNK_SIZE,
	.protocolVersion = 1u
};

GOS_STATIC svl_iplUserMsgDesc_t iplBinaryInfoReqMsg =
{
	.callback        = svl_sdhIplBinaryInfoReqCallback,
	.msgId           = 0x02
};

/**
 * SDH daemon descriptor.
 */
GOS_STATIC gos_taskDescriptor_t svlSdhTaskDesc =
{
    .taskFunction 	    = svl_sdhDaemon,
    .taskName 		    = "svl_sdh_daemon",
    .taskStackSize 	    = SVL_SDH_DAEMON_STACK_SIZE,
    .taskPriority 	    = SVL_SDH_DAEMON_PRIORITY,
    .taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: svl_sdhInit
 */
gos_result_t svl_sdhInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	// TODO:
	initResult = gos_sysmonRegisterUserMessage(&sysmonBinaryNumReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonBinaryInfoReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonDownloadReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonBinaryChunkReqMsg);

	initResult &= svl_iplRegisterUserMsg(&iplBinaryInfoReqMsg);
	initResult &= gos_taskRegister(&svlSdhTaskDesc, NULL);

	if (initResult != GOS_SUCCESS)
	{
		initResult = GOS_ERROR;
	}
	else
	{
		// OK.
	}

	return initResult;
}

/*
 * Function: svl_sdhConfigure
 */
gos_result_t svl_sdhConfigure (svl_sdhCfg_t* pCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t cfgResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (pCfg != NULL && pCfg->readFunction != NULL && pCfg->writeFunction != NULL)
	{
		sdhReadFunction  = pCfg->readFunction;
		sdhWriteFunction = pCfg->writeFunction;
		cfgResult        = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return cfgResult;
}

GOS_STATIC void_t svl_sdhDaemon (void_t)
{
	gos_message_t gosMsg = {0};
	gos_message_t gosRespMsg = {0};
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t msgIds [] = { SVL_SDH_STATE_CONT_MSG_ID, 0 };
	svl_sdhBinaryDesc_t newBinaryDescriptor = {0};
	u16_t chunkIndex = 0u;
	u16_t numOfChunks = 0u;

	for (;;)
	{
		if (gos_messageRx(msgIds, &gosMsg, 5000u) == GOS_SUCCESS)
		{
			(void_t) memcpy((void_t*)&controlMsg, (void_t*)gosMsg.messageBytes, sizeof(controlMsg));

			switch (controlMsg.requiredState)
			{
				case SDH_STATE_BINARY_NUM_REQ:
				{
					if (sdhReadFunction != NULL && sdhWriteFunction != NULL)
					{
#if SVL_SDH_TRACE_LEVEL > 0
					(void_t) gos_traceTrace(GOS_TRUE, "SDH binary number request received.\r\n");
#endif
						u16_t numOfBinaries = 0u;

						(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						if (numOfBinaries == 0xFFFF)
						{
							numOfBinaries = 0u;
							(void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));
						}
						else
						{
							// Number OK.
						}

						gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
						gosRespMsg.messageSize = sizeof(numOfBinaries);

						(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&numOfBinaries, sizeof(numOfBinaries));
						(void_t) gos_messageTx(&gosRespMsg);
					}
					break;
				}
				case SDH_STATE_BINARY_INFO_REQ:
				{
					if (sdhReadFunction != NULL)
					{
						u16_t index = 0u;
						u16_t numOfBinaries = 0u;
						svl_sdhBinaryDesc_t binaryDescriptor = {0};

						(void_t) memcpy((void_t*)&index, (void_t*)controlMsg.pData, sizeof(index));

#if SVL_SDH_TRACE_LEVEL > 0
					(void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary info request received. Index: %u\r\n", index);
#endif

						(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						(void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						if (index < numOfBinaries)
						{
							(void_t) sdhReadFunction(
									SVL_SDH_STORAGE_DESC_BIN_DESC_START + index * sizeof(svl_sdhBinaryDesc_t),
									(u8_t*)&binaryDescriptor,
									sizeof(binaryDescriptor)
									);

							gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
							gosRespMsg.messageSize = sizeof(binaryDescriptor);

							(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&binaryDescriptor, sizeof(binaryDescriptor));
							(void_t) gos_messageTx(&gosRespMsg);
						}
						else
						{
							// Invalid index.
						}
					}
					break;
				}
				case SDH_STATE_BINARY_DOWNLOAD_REQ:
				{
					if (sdhReadFunction != NULL && sdhState == SDH_STATE_IDLE)
					{
#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTrace(GOS_TRUE, "SDH binary download request received.\r\n");
#endif
						// Check if descriptor fits.
						u16_t numOfBinaries = 0u;
						u8_t  result;
						(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						if ((SVL_SDH_STORAGE_DESC_BIN_DESC_START + ((numOfBinaries + 1) * sizeof(svl_sdhBinaryDesc_t))) < SVL_SDH_STORAGE_DESC_AREA_SIZE)
						{
							// Check if binary fits.
							svl_sdhBinaryDesc_t binaryDescriptor = {0};

							(void_t) memcpy((void_t*)&newBinaryDescriptor, (void_t*)controlMsg.pData, sizeof(newBinaryDescriptor));

							if (numOfBinaries > 0)
							{
								(void_t) sdhReadFunction(
										SVL_SDH_STORAGE_DESC_BIN_DESC_START + (numOfBinaries - 1) * sizeof(svl_sdhBinaryDesc_t),
										(u8_t*)&binaryDescriptor,
										sizeof(binaryDescriptor)
										);

								newBinaryDescriptor.binaryLocation = binaryDescriptor.binaryLocation + binaryDescriptor.binaryInfo.size;

							}
							else
							{
								newBinaryDescriptor.binaryLocation = SVL_SDH_BINARY_AREA_START;
							}

#if SVL_SDH_TRACE_LEVEL == 2
							(void_t) gos_traceTraceFormatted(
									GOS_TRUE,
									"SDH new binary info:\r\n"
									"Name: %s\r\n"
									"Location: %u\r\n"
									"Size: %u\r\n"
									"Address: %u\r\n"
									"CRC: %u\r\n",
									newBinaryDescriptor.name,
									newBinaryDescriptor.binaryLocation,
									newBinaryDescriptor.binaryInfo.size,
									newBinaryDescriptor.binaryInfo.startAddress,
									newBinaryDescriptor.binaryInfo.crc);
#endif

							if ((newBinaryDescriptor.binaryLocation + newBinaryDescriptor.binaryInfo.size) < SVL_SDH_BINARY_AREA_END)
							{
								(void_t) sdhWriteFunction(
										SVL_SDH_STORAGE_DESC_BIN_DESC_START + numOfBinaries * sizeof(svl_sdhBinaryDesc_t),
										(u8_t*)&newBinaryDescriptor,
										sizeof(newBinaryDescriptor)
								);
								result = SDH_DOWNLOAD_REQ_OK;

								chunkIndex  = 0u;
								numOfChunks = newBinaryDescriptor.binaryInfo.size / SVL_SDH_CHUNK_SIZE + (newBinaryDescriptor.binaryInfo.size % SVL_SDH_CHUNK_SIZE == 0 ? 0 : 1);

								sdhState = SDH_STATE_DOWNLOADING_BINARY;
							}
							else
							{
								// New binary does not fit.
								result = SDH_DOWNLOAD_REQ_FILE_SIZE_ERR;
							}
						}
						else
						{
							// There is not enough size for descriptor.
							result = SDH_DOWNLOAD_REQ_DESC_SIZE_ERR;
						}

						gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
						gosRespMsg.messageSize = sizeof(result);

						(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&result, sizeof(result));
						(void_t) gos_messageTx(&gosRespMsg);
					}
					break;
				}
				case SDH_STATE_DOWNLOADING_BINARY:
				{
					if (sdhReadFunction != NULL && sdhWriteFunction != NULL && sdhState == SDH_STATE_DOWNLOADING_BINARY)
					{
						// Get chunk descriptor.
						svl_sdhChunkDesc_t chunkDesc = {0};
						(void_t) memcpy((void_t*)&chunkDesc, (void_t*)controlMsg.pData, sizeof(chunkDesc));

#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTrace(GOS_TRUE, "SDH binary chunk request received.\r\n");
#endif

#if SVL_SDH_TRACE_LEVEL == 2
						u32_t percentage = 100 * 100 * (chunkDesc.chunkIdx + 1) / numOfChunks;
						(void_t) gos_traceTraceFormatted(
								GOS_TRUE,
								"SDH chunk counter [%u/%u] ... %3u.%02u%%\r\n",
								chunkDesc.chunkIdx + 1,
								numOfChunks,
								percentage / 100,
								percentage % 100
						);
#endif

						// Save chunk.
						(void_t) sdhWriteFunction(
								newBinaryDescriptor.binaryLocation + chunkDesc.chunkIdx * SVL_SDH_CHUNK_SIZE,
								(u8_t*)(sdhBuffer + sizeof(chunkDesc)),
								SVL_SDH_CHUNK_SIZE
						);

						// Send response.
						chunkDesc.result = 1;

						gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
						gosRespMsg.messageSize = sizeof(chunkDesc);

						(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&chunkDesc, sizeof(chunkDesc));
						(void_t) gos_messageTx(&gosRespMsg);

						// Check if all chunks have been received.
						if (chunkDesc.chunkIdx == (numOfChunks - 1))
						{
#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTrace(GOS_TRUE, "SDH binary download finished.\r\n");
#endif
							u16_t numOfBinaries = 0u;

							(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));
							numOfBinaries++;
							(void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

							sdhState = SDH_STATE_IDLE;
						}
						else
						{
							// Continue reception.
						}
					}
					break;
				}
				default: break;
			}
		}
		else
		{
			// Timeout.
#if SVL_SDH_TRACE_LEVEL > 0
			if (sdhState != SDH_STATE_IDLE)
			{
				(void_t) gos_traceTrace(GOS_TRUE, "SDH timeout.\r\n");
			}
			else
			{
				// Just message RX timeout in idle state.
			}
#endif
			sdhState = SDH_STATE_IDLE;
			(void_t) gos_taskSleep(100);
		}
	}
}

GOS_STATIC void_t svl_sdhSysmonBinaryNumReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_NUM_REQ;

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB101,
				(void_t*)respMsg.messageBytes,
				sizeof(u16_t),
				0xFFFF);
	}
}

GOS_STATIC void_t svl_sdhSysmonBinaryInfoReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_INFO_REQ;
	controlMsg.pData = sdhBuffer;
	controlMsg.dataSize = sizeof(u16_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB102,
				(void_t*)respMsg.messageBytes,
				sizeof(svl_sdhBinaryDesc_t),
				0xFFFF);
	}
}

GOS_STATIC void_t svl_sdhSysmonDownloadReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_DOWNLOAD_REQ;
	controlMsg.pData = sdhBuffer;
	controlMsg.dataSize = sizeof(u32_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB103,
				(void_t*)respMsg.messageBytes,
				sizeof(u8_t),
				0xFFFF);
	}
}

GOS_STATIC void_t svl_sdhSysmonBinaryChunkReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_DOWNLOADING_BINARY;
	controlMsg.pData = sdhBuffer;
	controlMsg.dataSize = sizeof(svl_sdhChunkDesc_t) + SVL_SDH_CHUNK_SIZE;

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB104,
				(void_t*)respMsg.messageBytes,
				sizeof(svl_sdhChunkDesc_t),
				0xFFFF);
	}
}

GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	// TODO
#if 0
	/*
	 * Function code.
	 */
	(void_t) svl_iplSendMessage(0xA02, pData, dataLength);
#endif
}
