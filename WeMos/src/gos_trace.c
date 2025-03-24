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
//                                      (c) Ahmed Gazar, 2023
//
//*************************************************************************************************
//! @file       gos_trace.c
//! @author     Ahmed Gazar
//! @date       2023-11-21
//! @version    1.0
//!
//! @brief      GOS light trace service source.
//! @details    For a more detailed description of this service, please refer to @ref gos_trace.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2023-11-21    Ahmed Gazar     Initial version created
//*************************************************************************************************
//
// Copyright (c) 2023 Ahmed Gazar
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
#include <gos_trace.h>
#include <gos_time.h>
#include <gos_trace_driver.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <gos_task.h>

/*
 * Macros
 */
/**
 * Trace timestamp formatter.
 */
#define GOS_TRACE_TIMESTAMP_FORMAT       "["TRACE_FG_YELLOW_START"%04d-%02d-%02d %02d:%02d:%02d.%03d"TRACE_FORMAT_RESET"]\t"

/**
 * Trace timestamp length.
 */
#define GOS_TRACE_TIMESTAMP_LENGTH       ( 46u )

/*
 * Static variables
 */
/**
 * Trace formatted buffer for message formatting.
 */
GOS_STATIC char_t formattedBuffer [256];

/**
 * Buffer for timestamp printing.
 */
GOS_STATIC char_t timeStampBuffer [GOS_TRACE_TIMESTAMP_LENGTH];

/*
 * Function: gos_traceInit
 */
gos_result_t gos_traceInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t traceInitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    // Nothing to do in the light version.

    return traceInitResult;
}

/*
 * Function: gos_traceTrace
 */
GOS_INLINE gos_result_t gos_traceTrace (bool_t addTimeStamp, char_t* traceMessage)
{
    /*
     * Local variables.
     */
    gos_result_t             traceResult    = GOS_ERROR;
    gos_tid_t                callerTaskId   = GOS_INVALID_TASK_ID;
    gos_time_t               sysTime        = {0};
    gos_taskPrivilegeLevel_t privileges     = 0u;

    /*
     * Function code.
     */
    if ((traceMessage                                     != NULL           ) &&
        (gos_taskGetCurrentId(&callerTaskId)              == GOS_SUCCESS    ) &&
        (gos_taskGetPrivileges(callerTaskId, &privileges) == GOS_SUCCESS    ) &&
        ((privileges & GOS_PRIV_TRACE)                    == GOS_PRIV_TRACE )
        )
    {
        // Pre-set result.
        traceResult = GOS_SUCCESS;

        if (addTimeStamp == GOS_TRUE)
        {
            traceResult &= gos_timeGet(&sysTime);
            (void_t) sprintf(timeStampBuffer, "[" TRACE_FG_YELLOW_START "%04d-%02d-%02d %02d:%02d:%02d.%03d" TRACE_FORMAT_RESET "]\t",
                    sysTime.years,
                    sysTime.months,
                    sysTime.days,
                    sysTime.hours,
                    sysTime.minutes,
                    sysTime.seconds,
                    sysTime.milliseconds
                    );
            // Add timestamp to queue.
            traceResult &= gos_traceDriverTransmitString(timeStampBuffer);
        }
        else
        {
            // Timestamp not needed.
        }

        // Add trace message to queue.
        traceResult &= gos_traceDriverTransmitString(traceMessage);

        if (traceResult != GOS_SUCCESS)
        {
            traceResult = GOS_ERROR;
        }
        else
        {
            // Nothing to do.
        }
    }
    else
    {
        // Nothing to do.
    }

    return traceResult;
}

/*
 * Function: gos_traceTraceFormatted
 */
gos_result_t gos_traceTraceFormatted (bool_t addTimeStamp, GOS_CONST char_t* traceFormat, ...)
{
    /*
     * Local variables.
     */
    gos_result_t             traceResult    = GOS_SUCCESS;
    gos_tid_t                callerTaskId   = GOS_INVALID_TASK_ID;
    gos_time_t               sysTime        = {0};
    va_list                  args;
    gos_taskPrivilegeLevel_t privileges     = 0u;

    /*
     * Function code.
     */
    if ((traceFormat                                      != NULL           ) &&
        (gos_taskGetCurrentId(&callerTaskId)              == GOS_SUCCESS    ) &&
        (gos_taskGetPrivileges(callerTaskId, &privileges) == GOS_SUCCESS    ) &&
        ((privileges & GOS_PRIV_TRACE)                    == GOS_PRIV_TRACE )
        )
    {
        if (addTimeStamp == GOS_TRUE)
        {
            traceResult &= gos_timeGet(&sysTime);
            (void_t) sprintf(timeStampBuffer, GOS_TRACE_TIMESTAMP_FORMAT,
                    sysTime.years,
                    sysTime.months,
                    sysTime.days,
                    sysTime.hours,
                    sysTime.minutes,
                    sysTime.seconds,
                    sysTime.milliseconds
                    );
            // Add timestamp to queue.
            traceResult &= gos_traceDriverTransmitString(timeStampBuffer);
        }
        else
        {
            // Timestamp not needed.
        }

        // Add trace message to queue.
        va_start(args, traceFormat);
        (void_t) vsprintf(formattedBuffer, traceFormat, args);
        va_end(args);
        traceResult &= gos_traceDriverTransmitString(formattedBuffer);

        if (traceResult != GOS_SUCCESS)
        {
            traceResult = GOS_ERROR;
        }
        else
        {
            // Nothing to do.
        }
    }
    else
    {
        traceResult = GOS_ERROR;
    }

    return traceResult;
}

/*
 * Function: gos_traceTraceFormattedUnsafe
 */
gos_result_t gos_traceTraceFormattedUnsafe (GOS_CONST char_t* traceFormat, ...)
{
    /*
     * Local variables.
     */
    gos_result_t traceResult = GOS_ERROR;
    va_list      args;

    /*
     * Function code.
     */
    if (traceFormat != NULL)
    {
        va_start(args, traceFormat);
        (void_t) vsprintf(formattedBuffer, traceFormat, args);
        va_end(args);

        traceResult = gos_traceDriverTransmitString_Unsafe(formattedBuffer);
    }
    else
    {
        // Nothing to do.
    }

    return traceResult;
}
