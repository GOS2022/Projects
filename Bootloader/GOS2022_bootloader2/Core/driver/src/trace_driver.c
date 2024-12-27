/*
 * trace_driver.c
 *
 *  Created on: Oct 21, 2023
 *      Author: Gabor
 */
#include "trace_driver.h"
#include <string.h>

#define TRACE_MESSAGE_MAX_LENGTH ( 80u )
#define TRACE_ENTRY_MAX_NUM      ( 24u )

typedef struct
{
	char_t       message [TRACE_MESSAGE_MAX_LENGTH];
	gos_result_t result;
}trace_entry_t;

GOS_STATIC trace_entry_t traceEntries [TRACE_ENTRY_MAX_NUM];
GOS_STATIC u8_t          nextEntryIndex = 0u;

gos_result_t trace_driverEnqueueTraceMessage (const char_t* message, gos_result_t result)
{
	if (nextEntryIndex < TRACE_ENTRY_MAX_NUM)
	{
		(void_t) strcpy(traceEntries[nextEntryIndex].message, message);
		traceEntries[nextEntryIndex].result = result;
		nextEntryIndex++;
	}
	else
	{
		// Trace queue full.
	}

	return result;
}

void_t trace_driverFlushTraceEntries (void_t)
{
	u8_t traceIndex = 0u;

	for (traceIndex = 0u; traceIndex < nextEntryIndex; traceIndex++)
	{
		(void_t) gos_errorTraceInit(traceEntries[traceIndex].message, traceEntries[traceIndex].result);
	}

	nextEntryIndex = 0u;
}
