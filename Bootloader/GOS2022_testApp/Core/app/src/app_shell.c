/*
 * app_shell.c
 *
 *  Created on: Jul 15, 2026
 *      Author: gabor
 */


#include "app.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PARAM_LEN ( 16u )
#define MAX_PARAM_NUM ( 4u )

typedef enum
{
	CMD_PROJ_CFG = 0x2000,
	CMD_HELP     = 0x2001,
	CMD_GET_APPS = 0x2002,
	CMD_HEX_DUMP = 0x2003,
}app_shellMsgId_t;

typedef struct
{
	u8_t paramCount;
	char_t params [MAX_PARAM_NUM][MAX_PARAM_LEN];
}app_shellParam_t;

GOS_STATIC void_t app_shellTask       (void_t);
GOS_STATIC void_t app_shellProjCfgHandler (char_t** params);
GOS_STATIC void_t app_shellHelpHandler (char_t** params);
GOS_STATIC void_t app_shellGetAppsHandler (char_t** params);
GOS_STATIC void_t app_shellHexDumpHandler (char_t** params);
GOS_STATIC void_t app_shellExtractParameters (char_t** params, app_shellParam_t* pParam);

GOS_STATIC gos_tid_t shellTaskId;

GOS_STATIC gos_shellCommand_t commands [] =
{
	{
		.command = "proj_cfg",
		.commandHandler = app_shellProjCfgHandler,
		.commandPrivilege = GOS_SHELL_USER_PRIVILEGE_USER
	},
	{
		.command = "help",
		.commandHandler = app_shellHelpHandler,
		.commandPrivilege = GOS_SHELL_USER_PRIVILEGE_USER
	},
	{
		.command = "get_apps",
		.commandHandler = app_shellGetAppsHandler,
		.commandPrivilege = GOS_SHELL_USER_PRIVILEGE_USER
	},
	{
		.command = "hex_dump",
		.commandHandler = app_shellHexDumpHandler,
		.commandPrivilege = GOS_SHELL_USER_PRIVILEGE_USER
	}
};

GOS_STATIC gos_taskDescriptor_t app_shellTaskDesc =
{
	.taskFunction	    = app_shellTask,
	.taskName 		    = "app_shellTask",
	.taskStackSize 	    = 0x1000,
	.taskPriority 	    = 60,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

gos_result_t app_shellInit (void_t)
{
	gos_result_t shellInitResult = GOS_SUCCESS;

	shellInitResult &= gos_shellRegisterCommands(commands, sizeof(commands));
	shellInitResult &= gos_shellRegisterUser("admin", "admin123", GOS_SHELL_USER_PRIVILEGE_USER);
	shellInitResult &= gos_taskRegister(&app_shellTaskDesc, &shellTaskId);

	if (shellInitResult != GOS_SUCCESS)
	{
		shellInitResult = GOS_ERROR;
	}

	return shellInitResult;
}

GOS_STATIC void_t app_shellTask (void_t)
{
	gos_messageId_t msgIdArray [] = { CMD_PROJ_CFG, CMD_HELP, CMD_GET_APPS, CMD_HEX_DUMP, 0 };
	gos_message_t rxMessage;
	app_shellParam_t param;

	for (;;)
	{
		if (gos_messageRx(msgIdArray, &rxMessage, GOS_MESSAGE_ENDLESS_TMO) == GOS_SUCCESS)
		{
			(void_t) gos_shellDriverTransmitString("\r\n");

			// Process message.
			switch (rxMessage.messageId)
			{
				case CMD_PROJ_CFG:
				{
					svl_pdhSwInfo_t swInfo;
					svl_pdhGetSwInfo(&swInfo);
					memcpy((void_t*)&param, (void_t*)rxMessage.messageBytes, sizeof(app_shellParam_t));

					if (param.paramCount == 0)
					{
						(void_t) gos_shellDriverTransmitString("Software information: \r\n");
						(void_t) gos_shellDriverTransmitString("Application name: %s\r\n", swInfo.appSwVerInfo.name);
						(void_t) gos_shellDriverTransmitString("Application OS version: %02u.%02u\r\n", swInfo.appOsInfo.major, swInfo.appOsInfo.minor);
						(void_t) gos_shellDriverTransmitString("Application version: %02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build);
						(void_t) gos_shellDriverTransmitString("Application author: %s\r\n", swInfo.appSwVerInfo.author);
						(void_t) gos_shellDriverTransmitString("Application date: %04u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days);
						(void_t) gos_shellDriverTransmitString("Application description: %s\r\n", swInfo.appSwVerInfo.description);
					}
					else if (strcmp(param.params[0], "-app_ver") == 0)
					{
						(void_t) gos_shellDriverTransmitString("Application version: %02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build);
					}
					else if (strcmp(param.params[0], "-app_date") == 0)
					{
						(void_t) gos_shellDriverTransmitString("Application date: %04u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days);
					}
					else
					{
						(void_t) gos_shellDriverTransmitString("Invalid parameter. See: help -proj_cfg for more information.\r\n");
					}
					break;
				}
				case CMD_HELP:
				{
					memcpy((void_t*)&param, (void_t*)rxMessage.messageBytes, sizeof(app_shellParam_t));

					if (param.paramCount == 0)
					{
						(void_t) gos_shellDriverTransmitString("List of available user commands: \r\n");
				        (void_t) gos_shellDriverTransmitString("\t"
				                "- help\r\n\t"
				                "- proj_cfg\r\n");
					}
					else if (strcmp(param.params[0], "proj_cfg") == 0)
					{
						(void_t) gos_shellDriverTransmitString("Prints the project configuration. The applicable parameters are:\r\n\t"
								"-app_ver\t\tPrints the application software version.\r\n\t"
								"-app_date\t\tPrints the application software date.\r\n");
					}
					break;
				}
				case CMD_GET_APPS:
				{
					u16_t index = 0u;
					svl_sdhBinaryDesc_t binaryDesc;
					gos_result_t res = GOS_SUCCESS;

					//memcpy((void_t*)&param, (void_t*)rxMessage.messageBytes, sizeof(app_shellParam_t));

					while (res == GOS_SUCCESS)
					{
						(void_t) memset(&binaryDesc, 0, sizeof(binaryDesc));
						res = svl_sdhGetBinaryData(index++, &binaryDesc);

						if (res == GOS_SUCCESS)
						{
							(void_t) gos_shellDriverTransmitString("-----------------------------\r\n");
							(void_t) gos_shellDriverTransmitString("Index: %u\r\n", index);
							(void_t) gos_shellDriverTransmitString("File name: %s\r\n", binaryDesc.name);
							(void_t) gos_shellDriverTransmitString("Install date: %4u-%02d-%02d\r\n",
									binaryDesc.installDate.years, binaryDesc.installDate.months, binaryDesc.installDate.days);
							(void_t) gos_shellDriverTransmitString("File size: %u bytes\r\n", binaryDesc.binaryInfo.size);
							(void_t) gos_shellDriverTransmitString("Start address: 0x%08X\r\n", binaryDesc.binaryInfo.startAddress);
							(void_t) gos_shellDriverTransmitString("CRC: 0x%08X\r\n", binaryDesc.binaryInfo.crc);
							(void_t) gos_shellDriverTransmitString("File location: 0x%08X\r\n", binaryDesc.binaryLocation);

							(void_t) gos_shellDriverTransmitString("-----------------------------\r\n");
						}
					}
					break;
				}
				case CMD_HEX_DUMP:
				{
					u16_t index;
					svl_sdhBinaryDesc_t binaryDesc;
					svl_sdhGetBinaryData(index, &binaryDesc);
					u32_t address;
					u32_t startAddress;
					u32_t endAddress;
					u32_t startOffset = 0u;
#define READ_SIZE ( 32 )
					u8_t byteBuffer [READ_SIZE];

					memcpy((void_t*)&param, (void_t*)rxMessage.messageBytes, sizeof(app_shellParam_t));

					if (param.paramCount == 0)
					{
						break;
					}

					index = atoi(param.params[0]);

					if (param.paramCount == 3)
					{
						u32_t reqStart = (u32_t)strtol(param.params[1], NULL, 0);
						u32_t reqEnd   = (u32_t)strtol(param.params[2], NULL, 0);
						startAddress = binaryDesc.binaryLocation + reqStart;
						endAddress = startAddress + (reqEnd - reqStart);
					}
					else
					{
						startAddress = binaryDesc.binaryLocation;
						endAddress = binaryDesc.binaryLocation + binaryDesc.binaryInfo.size;
					}

					for (address = startAddress; address < endAddress; address += READ_SIZE)
					{
						svl_sdhReadBytesFromMemory(address, byteBuffer, READ_SIZE);

						for (u8_t idx = 0; idx < READ_SIZE; idx++)
						{
							(void_t)gos_shellDriverTransmitString("%02X ", byteBuffer[idx]);
						}

						(void_t)gos_shellDriverTransmitString("\r\n");
					}
					break;
				}
			}

			gos_shellForcePrintPrompt();
		}
	}
}

GOS_STATIC void_t app_shellProjCfgHandler (char_t** params)
{
	gos_message_t message;
	app_shellParam_t param;
	app_shellExtractParameters(params, &param);

	message.messageId = CMD_PROJ_CFG;
	memcpy(message.messageBytes, (void_t*)&param, sizeof(param));
	message.messageSize = sizeof(param);
	gos_messageTx(&message);
}

GOS_STATIC void_t app_shellHelpHandler (char_t** params)
{
	gos_message_t message;
	app_shellParam_t param;
	app_shellExtractParameters(params, &param);

	message.messageId = CMD_HELP;
	memcpy(message.messageBytes, (void_t*)&param, sizeof(param));
	message.messageSize = sizeof(param);
	gos_messageTx(&message);
}

GOS_STATIC void_t app_shellGetAppsHandler (char_t** params)
{
	gos_message_t message;
	message.messageId = CMD_GET_APPS;
	message.messageSize = 0u;
	gos_messageTx(&message);
}

GOS_STATIC void_t app_shellHexDumpHandler (char_t** params)
{
	gos_message_t message;
	app_shellParam_t param;
	app_shellExtractParameters(params, &param);

	message.messageId = CMD_HEX_DUMP;
	memcpy(message.messageBytes, (void_t*)&param, sizeof(param));
	message.messageSize = sizeof(param);
	gos_messageTx(&message);
}

GOS_STATIC void_t app_shellExtractParameters (char_t** params, app_shellParam_t* pParam)
{
	u8_t index = 0u;

	if (pParam == NULL)
	{
		return;
	}

	(void_t) memset(pParam, 0, sizeof(*pParam));

	if (params == NULL)
	{
		return;
	}

	while (params[index] != NULL)
	{
		if (strcmp(params[index], "") == 0)
		{
			break;
		}
		else
		{
			strcpy(pParam->params[pParam->paramCount], params[index]);
			pParam->paramCount++;
		}
		index++;
	}
}
