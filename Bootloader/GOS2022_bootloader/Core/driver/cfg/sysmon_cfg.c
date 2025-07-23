/*
 * sysmon_cfg.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Ahmed
 */
#include <app.h>
#include <gos_lib.h>

// TODO
GOS_CONST svl_sysmonServiceConfig_t sysmonConfig =
{
	.wiredChannelNumber    = 0u,
	.wiredTxFunction       = drv_sysmonWiredTransmit,
	.wiredRxFunction       = drv_sysmonWiredReceive,
	.wirelessChannelNumber = 1u,
	.wirelessTxFunction    = drv_sysmonWirelessTransmit,
	.wirelessRxFunction    = drv_sysmonWirelessReceive
};
