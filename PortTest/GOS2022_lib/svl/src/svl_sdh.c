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
#include <drv_crc.h>
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
/**
 * SDH states.
 */
typedef enum
{
	SDH_STATE_IDLE,               //!< Idle state.
	SDH_STATE_DOWNLOADING_BINARY, //!< Downloading binary.

	SDH_STATE_BINARY_NUM_REQ,     //!< Number of binaries request.
	SDH_STATE_BINARY_INFO_REQ,    //!< Binary info request.
	SDH_STATE_BINARY_DOWNLOAD_REQ,//!< Binary download request.
	SDH_STATE_BINARY_INSTALL_REQ, //!< Binary install request.
	SDH_STATE_BINARY_ERASE_REQ    //!< Binary erase request.
}svl_sdhState_t;

/**
 * SDH download request results.
 */
typedef enum
{
	SDH_DOWNLOAD_REQ_OK = 1,            //!< Request OK.
	SDH_DOWNLOAD_REQ_DESC_SIZE_ERR = 2, //!< Descriptor does not fit.
	SDH_DOWNLOAD_REQ_FILE_SIZE_ERR = 4  //!< File does not fit.
}svl_sdhDownloadReqRes_t;

/**
 * SDH control message.
 */
typedef struct
{
	svl_sdhState_t requiredState;
	u8_t*          pData;
	u16_t          dataSize;
}svl_sdhControlMsg_t;

/**
 * SDH chunk descriptor.
 */
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

/**
 * SDH buffer for sysmon messages.
 */
GOS_STATIC u8_t  sdhBuffer [SVL_SDH_BUFFER_SIZE];

/**
 * State control message.
 */
GOS_STATIC gos_message_t sdhStateControlMsg =
{
	.messageId   = SVL_SDH_STATE_CONT_MSG_ID,
	.messageSize = sizeof(svl_sdhControlMsg_t)
};

/**
 * SDH state-machine variable.
 */
GOS_STATIC svl_sdhState_t sdhState = SDH_STATE_IDLE;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_sdhDaemon (void_t);

GOS_STATIC void_t svl_sdhSysmonBinaryNumReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryInfoReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonDownloadReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryChunkReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonSoftwareInstallReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryEraseReqCallback (void_t);

GOS_STATIC void_t svl_sdhIplBinaryNumReqCallback (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplDownloadReqCallback (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryChunkReqCallback (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplSoftwareInstallReqCallback (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryEraseReqCallback (u8_t* pData, u32_t size, u32_t crc);

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

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonSoftwareInstallReqMsg =
{
	.callback        = svl_sdhSysmonSoftwareInstallReqCallback,
	.messageId       = 0x2105,
	.payload         = (void_t*)sdhBuffer,
	.payloadSize     = sizeof(u16_t),
	.protocolVersion = 1u
};

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryEraseReqMsg =
{
	.callback        = svl_sdhSysmonBinaryEraseReqCallback,
	.messageId       = 0x2106,
	.payload         = (void_t*)sdhBuffer,
	.payloadSize     = sizeof(u16_t) + sizeof(bool_t),
	.protocolVersion = 1u
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryNumReqMsg =
{
	.callback        = svl_sdhIplBinaryNumReqCallback,
	.msgId           = 0x02
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryInfoReqMsg =
{
	.callback        = svl_sdhIplBinaryInfoReqCallback,
	.msgId           = 0x12
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplDownloadReqMsg =
{
	.callback        = svl_sdhIplDownloadReqCallback,
	.msgId           = 0x22
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryChunkReqMsg =
{
	.callback        = svl_sdhIplBinaryChunkReqCallback,
	.msgId           = 0x32
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplSoftwareInstallReqMsg =
{
	.callback        = svl_sdhIplSoftwareInstallReqCallback,
	.msgId           = 0x42
};

/**
 * TODO
 */
GOS_STATIC svl_iplUserMsgDesc_t iplEraseReqMsg =
{
	.callback        = svl_sdhIplBinaryEraseReqCallback,
	.msgId           = 0x52
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
	initResult = gos_sysmonRegisterUserMessage(&sysmonBinaryNumReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonBinaryInfoReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonDownloadReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonBinaryChunkReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonSoftwareInstallReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&sysmonBinaryEraseReqMsg);

	initResult &= svl_iplRegisterUserMsg(&iplBinaryNumReqMsg);
	initResult &= svl_iplRegisterUserMsg(&iplBinaryInfoReqMsg);
	initResult &= svl_iplRegisterUserMsg(&iplDownloadReqMsg);
	initResult &= svl_iplRegisterUserMsg(&iplBinaryChunkReqMsg);
	initResult &= svl_iplRegisterUserMsg(&iplSoftwareInstallReqMsg);
	initResult &= svl_iplRegisterUserMsg(&iplEraseReqMsg);

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

/*
 * Function: svl_sdhGetBinaryData
 */
gos_result_t svl_sdhGetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc)
{
	/*
	 * Local variables.
	 */
	gos_result_t dataGetResult = GOS_SUCCESS;
	u16_t        numOfBinaries = 0u;

	/*
	 * Function code.
	 */
	if (pDesc != NULL && sdhReadFunction != NULL)
	{
		(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

		if (index < numOfBinaries)
		{
			(void_t) sdhReadFunction(
					SVL_SDH_STORAGE_DESC_BIN_DESC_START + index * sizeof(svl_sdhBinaryDesc_t),
					(u8_t*)pDesc,
					sizeof(*pDesc)
					);
		}
		else
		{
			// Invalid request.
			dataGetResult = GOS_ERROR;
		}
	}
	else
	{
		// NULL pointer error.
		dataGetResult = GOS_ERROR;
	}

	return dataGetResult;
}

/*
 * Function: svl_sdhSetBinaryData
 */
gos_result_t svl_sdhSetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc)
{
	/*
	 * Local variables.
	 */
	gos_result_t dataSetResult = GOS_SUCCESS;
	u16_t        numOfBinaries = 0u;

	/*
	 * Function code.
	 */
	if (pDesc != NULL && sdhReadFunction != NULL && sdhWriteFunction != NULL)
	{
		(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

		if (index < numOfBinaries)
		{
			(void_t) sdhWriteFunction(
					SVL_SDH_STORAGE_DESC_BIN_DESC_START + index * sizeof(svl_sdhBinaryDesc_t),
					(u8_t*)pDesc,
					sizeof(*pDesc)
					);
		}
		else
		{
			// Invalid request.
			dataSetResult = GOS_ERROR;
		}
	}
	else
	{
		// NULL pointer error.
		dataSetResult = GOS_ERROR;
	}

	return dataSetResult;
}

/*
 * Function: svl_sdhReadBytesFromMemory
 */
gos_result_t svl_sdhReadBytesFromMemory (u32_t address, u8_t* pBuffer, u32_t size)
{
	/*
	 * Local variables.
	 */
	gos_result_t readResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	if (pBuffer != NULL && sdhReadFunction != NULL)
	{
		(void_t) sdhReadFunction(address, pBuffer, size);
	}
	else
	{
		// NULL pointer error.
		readResult = GOS_ERROR;
	}

	return readResult;
}

GOS_STATIC void_t svl_sdhDaemon (void_t)
{
	gos_message_t gosMsg = {0};
	gos_message_t gosRespMsg = {0};
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t msgIds [] = { SVL_SDH_STATE_CONT_MSG_ID, 0 };
	svl_sdhBinaryDesc_t newBinaryDescriptor = {0};
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
					else
					{
						// Request cannot be served.
					}
					break;
				}
				case SDH_STATE_BINARY_INFO_REQ:
				{
					if (sdhReadFunction != NULL)
					{
						u16_t index = 0u;
						svl_sdhBinaryDesc_t binaryDescriptor = {0};

						(void_t) memcpy((void_t*)&index, (void_t*)controlMsg.pData, sizeof(index));

#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary info request received. Index: %u\r\n", index);
#endif
						if (svl_sdhGetBinaryData(index, &binaryDescriptor) == GOS_SUCCESS)
						{
							gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
							gosRespMsg.messageSize = sizeof(binaryDescriptor);

							(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&binaryDescriptor, sizeof(binaryDescriptor));
							(void_t) gos_messageTx(&gosRespMsg);
						}
						else
						{
							// Wrong request.
						}
					}
					else
					{
						// Request cannot be served.
					}
					break;
				}
				case SDH_STATE_BINARY_INSTALL_REQ:
				{
					if (sdhReadFunction != NULL && sdhState == SDH_STATE_IDLE)
					{
						u16_t index = 0u;
						u16_t numOfBinaries = 0u;
						svl_pdhBldCfg_t bldCfg = {0};

						(void_t) memcpy((void_t*)&index, (void_t*)controlMsg.pData, sizeof(index));

#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary install request received. Index: %u\r\n", index);
#endif

						(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						if (index < numOfBinaries)
						{
							(void_t) svl_pdhGetBldCfg(&bldCfg);

							bldCfg.installRequested = GOS_TRUE;
							bldCfg.binaryIndex      = index;

							(void_t) svl_pdhSetBldCfg(&bldCfg);

							gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
							gosRespMsg.messageSize = sizeof(u16_t);

							(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&index, sizeof(index));
							(void_t) gos_messageTx(&gosRespMsg);
						}
					}
					else
					{
						// Request cannot be served.
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
					else
					{
						// Request cannot be served.
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
								(u8_t*)(controlMsg.pData + sizeof(chunkDesc)),
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
				case SDH_STATE_BINARY_ERASE_REQ:
				{
					if (sdhReadFunction != NULL && sdhWriteFunction != NULL && sdhState == SDH_STATE_IDLE)
					{
						u16_t index = 0u;
						u16_t numOfBinaries = 0u;
						svl_sdhBinaryDesc_t binaryDescriptor = {0};
						u32_t startAddress = 0u;
						u32_t fromAddress  = 0u;
						u32_t totalCopySize = 0u;
						bool_t defragment = GOS_FALSE;

						(void_t) memcpy((void_t*)&index, (void_t*)controlMsg.pData, sizeof(index));
						(void_t) memcpy((void_t*)&defragment, (void_t*)controlMsg.pData + sizeof(index), sizeof(defragment));
#if SVL_SDH_TRACE_LEVEL > 0
						(void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary erase request received. Index: %u\r\n", index);
#endif

						(void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

						if (index < numOfBinaries)
						{
							svl_sdhGetBinaryData(index, &binaryDescriptor);

							// Start address to copy is the binary location of
							// the binary that is being deleted.
							startAddress = binaryDescriptor.binaryLocation;

							// Reorganize descriptors and binary.
							if (index != (numOfBinaries - 1))
							{
								// Move descriptors.
								for (u16_t idx = 0u; idx < (numOfBinaries - (index + 1)); idx++)
								{
									svl_sdhGetBinaryData((index + 1 + idx), &binaryDescriptor);
									svl_sdhSetBinaryData((index + idx), &binaryDescriptor);
								}

								// Move binaries and update descriptor data.
								// Prepare variables.
								if (defragment == GOS_TRUE)
								{
									svl_sdhGetBinaryData(index, &binaryDescriptor);
									fromAddress = binaryDescriptor.binaryLocation;

									svl_sdhGetBinaryData((numOfBinaries - 2), &binaryDescriptor);
									totalCopySize = binaryDescriptor.binaryLocation + binaryDescriptor.binaryInfo.size - fromAddress;

									// Move in chunks using SDH buffer.
									for (u32_t cntr = 0u; (cntr * 2048) < totalCopySize; cntr++)
									{
										if ((cntr + 1) * 2048 < totalCopySize)
										{
											(void_t) sdhReadFunction(fromAddress + (cntr * 2048), sdhBuffer, 2048);
											(void_t) sdhWriteFunction(startAddress + (cntr * 2048), sdhBuffer, 2048);
										}
										else
										{
											(void_t) sdhReadFunction(fromAddress + (cntr * 2048), sdhBuffer, totalCopySize - (cntr * 2048));
											(void_t) sdhWriteFunction(startAddress + (cntr * 2048), sdhBuffer, totalCopySize - (cntr * 2048));
										}
									}
								}
								else
								{
									// No defragmentation needed.
								}
							}
							else
							{
								// No moving required.
							}

							// Decrease number of binaries.
							numOfBinaries--;
							(void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

							gosRespMsg.messageId = SVL_SDH_STATE_CONT_RESP_MSG_ID;
							gosRespMsg.messageSize = sizeof(u16_t);

							(void_t) memcpy((void_t*)gosRespMsg.messageBytes, (void_t*)&index, sizeof(index));
							(void_t) gos_messageTx(&gosRespMsg);
						}
						else
						{
							// Wrong request.
						}
					}
					else
					{
						// Request cannot be served.
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
	else
	{
		// Nothing to do.
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
	else
	{
		// Nothing to do.
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
	else
	{
		// Nothing to do.
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
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhSysmonSoftwareInstallReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_INSTALL_REQ;
	controlMsg.pData = sdhBuffer;
	controlMsg.dataSize = sizeof(u16_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB105,
				(void_t*)respMsg.messageBytes,
				sizeof(u16_t),
				0xFFFF);
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhSysmonBinaryEraseReqCallback (void_t)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_ERASE_REQ;
	controlMsg.pData = sdhBuffer;
	controlMsg.dataSize = sizeof(u16_t) + sizeof(bool_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 300000) == GOS_SUCCESS)
	{
		(void_t) gos_gcpTransmitMessage(
	    		CFG_SYSMON_GCP_CHANNEL_NUM,
				0xB106,
				(void_t*)respMsg.messageBytes,
				sizeof(u16_t),
				0xFFFF);
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhIplBinaryNumReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_NUM_REQ;

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) svl_iplSendMessage(0xA02, (u8_t*)respMsg.messageBytes, sizeof(u16_t));
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_INFO_REQ;
	controlMsg.pData = pData;
	controlMsg.dataSize = size; //sizeof(u16_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) svl_iplSendMessage(0xA12, (u8_t*)respMsg.messageBytes, sizeof(svl_sdhBinaryDesc_t));
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhIplDownloadReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_DOWNLOAD_REQ;
	controlMsg.pData = pData;
	controlMsg.dataSize = size; //sizeof(u32_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) svl_iplSendMessage(0xA22, (u8_t*)respMsg.messageBytes, sizeof(u8_t));
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhIplBinaryChunkReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	if (drv_crcCheckCrc32(pData, size, crc, NULL) != DRV_CRC_CHECK_OK)
	{
		svl_sdhChunkDesc_t chunkDesc = { .result = 0 };
		(void_t) svl_iplSendMessage(0xA32, (u8_t*)&chunkDesc, sizeof(svl_sdhChunkDesc_t));
	}
	else
	{
		(void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

		controlMsg.requiredState = SDH_STATE_DOWNLOADING_BINARY;
		controlMsg.pData = sdhBuffer;
		controlMsg.dataSize = size; //sizeof(svl_sdhChunkDesc_t) + SVL_SDH_CHUNK_SIZE;

		(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

		(void_t) gos_messageTx(&sdhStateControlMsg);

		if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
		{
			(void_t) svl_iplSendMessage(0xA32, (u8_t*)respMsg.messageBytes, sizeof(svl_sdhChunkDesc_t));
		}
		else
		{
			// Nothing to do.
		}
	}
}

GOS_STATIC void_t svl_sdhIplSoftwareInstallReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_INSTALL_REQ;
	controlMsg.pData = pData;
	controlMsg.dataSize = size; //sizeof(u16_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 3000) == GOS_SUCCESS)
	{
		(void_t) svl_iplSendMessage(0xA42, (u8_t*)respMsg.messageBytes, sizeof(u16_t));
	}
	else
	{
		// Nothing to do.
	}
}

GOS_STATIC void_t svl_sdhIplBinaryEraseReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
	svl_sdhControlMsg_t controlMsg = {0};
	gos_messageId_t respMsgId [] = { SVL_SDH_STATE_CONT_RESP_MSG_ID, 0 };
	gos_message_t respMsg = {0};

	controlMsg.requiredState = SDH_STATE_BINARY_ERASE_REQ;
	controlMsg.pData = pData;
	controlMsg.dataSize = size; //sizeof(u16_t) + sizeof(bool_t);

	(void_t) memcpy((void_t*)sdhStateControlMsg.messageBytes, (void_t*)&controlMsg, sizeof(controlMsg));

	(void_t) gos_messageTx(&sdhStateControlMsg);

	if (gos_messageRx(respMsgId, &respMsg, 300000) == GOS_SUCCESS)
	{
		(void_t) svl_iplSendMessage(0xA52, (u8_t*)respMsg.messageBytes, sizeof(u16_t));
	}
	else
	{
		// Nothing to do.
	}
}
