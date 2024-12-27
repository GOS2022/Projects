/*
 * trace_driver.h
 *
 *  Created on: Oct 21, 2023
 *      Author: Gabor
 */

#ifndef DRIVER_INC_TRACE_DRIVER_H_
#define DRIVER_INC_TRACE_DRIVER_H_

#include "gos.h"

gos_result_t trace_driverEnqueueTraceMessage (const char_t* message, gos_result_t result);

void_t trace_driverFlushTraceEntries (void_t);

#endif /* DRIVER_INC_TRACE_DRIVER_H_ */
