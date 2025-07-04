/*
 * mdi_cfg.c
 *
 *  Created on: Jan 27, 2025
 *      Author: Ahmed
 */
#include <gos_lib.h>
#include <ers_def.h>

/**
 * TODO
 */
svl_ersEventDesc_t ersEvents [] =
{
	[ERS_STARTUP_EVENT] =
	{
		.description = "Startup event",
	},
	[ERS_OVERHEAT_EVENT] =
	{
		.description = "CPU over-heat event",
	}
};

/**
 * TODO
 */
u32_t ersEventsSize = sizeof(ersEvents);
