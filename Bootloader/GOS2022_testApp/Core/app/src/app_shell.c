/*
 * app_shell.c
 *
 *  Created on: Jul 15, 2026
 *      Author: gabor
 */


#include "app.h"

GOS_STATIC void_t app_shellProjCfgHandler (char_t* params);
GOS_STATIC void_t app_shellHelpHandler (char_t* params);

GOS_STATIC gos_shellCommand_t commands [] =
{
	{
		.command = "proj_cfg",
		.commandHandler = app_shellProjCfgHandler
	},
	{
		.command = "help",
		.commandHandler = app_shellHelpHandler
	}
};

gos_result_t app_shellInit (void_t)
{
	gos_result_t shellInitResult = GOS_SUCCESS;

	shellInitResult &= gos_shellRegisterCommands(commands, sizeof(commands));

	if (shellInitResult != GOS_SUCCESS)
	{
		shellInitResult = GOS_ERROR;
	}

	return shellInitResult;
}

GOS_STATIC void_t app_shellProjCfgHandler (char_t* params)
{
	svl_pdhSwInfo_t swInfo;
	svl_pdhGetSwInfo(&swInfo);
	if (strcmp(params, "") == 0)
	{
		(void_t) gos_shellDriverTransmitString("Software information: \r\n");
		(void_t) gos_shellDriverTransmitString("Application name: %s\r\n", swInfo.appSwVerInfo.name);
		(void_t) gos_shellDriverTransmitString("Application OS version: %02u.%02u\r\n", swInfo.appOsInfo.major, swInfo.appOsInfo.minor);
		(void_t) gos_shellDriverTransmitString("Application version: %02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build);
		(void_t) gos_shellDriverTransmitString("Application author: %s\r\n", swInfo.appSwVerInfo.author);
		(void_t) gos_shellDriverTransmitString("Application date: %04u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days);
		(void_t) gos_shellDriverTransmitString("Application description: %s\r\n", swInfo.appSwVerInfo.description);
	}
	else if (strcmp(params, "-app_ver") == 0)
	{
		(void_t) gos_shellDriverTransmitString("Application version: %02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build);
	}
	else if (strcmp(params, "-app_date") == 0)
	{
		(void_t) gos_shellDriverTransmitString("Application date: %04u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days);
	}
	else
	{
		(void_t) gos_shellDriverTransmitString("Invalid parameter. See: help -proj_cfg for more information.\r\n");
	}
}

GOS_STATIC void_t app_shellHelpHandler (char_t* params)
{
	if (strcmp(params, "") == 0)
	{
		(void_t) gos_shellDriverTransmitString("List of available user commands: \r\n");
        (void_t) gos_shellDriverTransmitString("\t"
                "- help\r\n\t"
                "- proj_cfg\r\n");
	}
	else if (strcmp(params, "proj_cfg") == 0)
	{
		(void_t) gos_shellDriverTransmitString("Prints the project configuration. The applicable parameters are:\r\n\t"
				"-app_ver\t\tPrints the application software version.\r\n\t"
				"-app_date\t\tPrints the application software date.\r\n");
	}
}
