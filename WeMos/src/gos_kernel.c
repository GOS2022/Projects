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
//                                      (c) Ahmed Gazar, 2022
//
//*************************************************************************************************
//! @file       gos_kernel.c
//! @author     Ahmed Gazar
//! @date       2024-04-24
//! @version    1.20
//!
//! @brief      GOS kernel source.
//! @details    For a more detailed description of this module, please refer to @ref gos_kernel.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2022-10-21    Ahmed Gazar     Initial version created.
// 1.1        2022-11-15    Ahmed Gazar     +    License added
// 1.2        2022-12-03    Ahmed Gazar     +    gos_kernelTaskRegisterTasks added
//                                          +    Tasks external task ID handling added
//                                          +    Kernel configuration structure array added
//                                          +    Cooperative scheduling introduced
// 1.3        2022-12-04    Ahmed Gazar     *    Sleep, wake-up, suspend, resume, block, unblock
//                                               hook functions removed
//                                          +    Kernel dump ready signal added
// 1.4        2022-12-08    Ahmed Gazar     +    Task priority setter and getter functions added
// 1.5        2022-12-15    Ahmed Gazar     +    Privilege handling introduced, privilege check
//                                               added to kernel functions
// 1.6        2023-01-13    Ahmed Gazar     *    Scheduling disabled flag replaced with counter
//                                          -    Configuration dump removed
// 1.7        2023-05-03    Ahmed Gazar     +    Stack overflow monitoring added
//                                          *    Dump separated to task and stack statistics
// 1.8        2023-05-19    Ahmed Gazar     +    gos_kernelGetCpuUsage added
// 1.9        2023-06-17    Ahmed Gazar     *    Kernel dump moved from task to function
//                                          *    CPU statistics fix
// 1.10       2023-06-30    Ahmed Gazar     +    cpuUseLimit added, CPU usage limit implemented
//                                          *    CPU statistics calculation moved to idle task
//                                          *    gos_kernelCalculateTaskCpuUsages reset required
//                                               flag added
//                                          +    gos_kernelSetMaxCpuLoad added
//                                          +    gos_kernelGetMaxCpuLoad added
//                                          +    gos_kernelIsCallerIsr added
//                                          +    gos_kernelIsCallerPrivileged added
//                                          +    gos_kernelTaskSetPrivileges  added
// 1.11       2023-07-11    Ahmed Gazar     -    privilegedAccess removed
//                                          +    kernelSysTickHookFunction added
//                                          -    Privileged access concept removed
//                                          +    Task block timeout introduced
// 1.12       2023-07-25    Ahmed Gazar     +    gos_kernelTaskGetDataByIndex added
//                                          *    Ported function calls added
// 1.13       2023-09-08    Ahmed Gazar     *    Sleep and block tick handling modified
// 1.14       2023-09-25    Ahmed Gazar     +    Missing else branches added
// 1.15       2023-10-20    Ahmed Gazar     +    isKernelRunning flag added
//                                          +    kernelPrivilegedHookFunction added
//                                          +    privilegedModeSetRequired flag added
//                                          +    gos_kernelPrivilegedModeSetRequired function added
//                                          +    Millisecond system time increment added to systick
//                                               handler
// 1.16       2023-11-01    Ahmed Gazar     *    Task-related definitions, variables, and functions
//                                               removed
//                                          +    inIsr and atomic counters, primask added
// 1.17       2023-11-10    Ahmed Gazar     +    Missing else branches and void casts added
// 1.18       2024-04-02    Ahmed Gazar     *    gos_kernelCalculateTaskCpuUsages: break from loop
//                                               moved to the beginning of loop
// 1.19       2024-04-17    Ahmed Gazar     *    Task block timeout check fixed
// 1.20       2024-04-24    Ahmed Gazar     -    Process service include removed
//*************************************************************************************************
//
// Copyright (c) 2022 Ahmed Gazar
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
#include <gos_error.h>
#include <gos_kernel.h>
#include <gos_task.h>
#include <gos_shell_driver.h>
#include <gos_timer_driver.h>
#include <stdio.h>
#include <string.h>

/*
 * Macros
 */
/**
 * Task dump separator line.
 */
#define TASK_DUMP_SEPARATOR     "+--------+------------------------------+------+------------------+-----------+\r\n"

/**
 * Pattern for binary format printing.
 */
#define BINARY_PATTERN          "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"TRACE_FORMAT_RESET

/**
 * u16_t to binary converter macro.
 */
#define TO_BINARY(word)  \
    (word & 0x8000 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x4000 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x2000 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x1000 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0800 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0400 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0200 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0100 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0080 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0040 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0020 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0010 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0008 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0004 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0002 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0"), \
    (word & 0x0001 ? TRACE_FG_GREEN_START"1" : TRACE_FG_RED_START"0")

/*
 * Global variables
 */
/**
 * Scheduling disabled counter.
 */
u8_t                                schedDisableCntr             = 0u;

/**
 * In ISR counter.
 */
u8_t                                inIsr                        = 0u;

/**
 * Atomic counter (incremented when GOS_ATOMIC_ENTER is called).
 */
u8_t                                atomicCntr                   = 0u;

/**
 * IRQ state for restoring after GOS_ATOMIC_EXIT.
 */
u32_t                               primask                      = 0u;

/**
 * Current task index - for priority scheduling.
 */
u32_t                               currentTaskIndex             = 0u;

/**
 * CPU use limit.
 */
u16_t                               cpuUseLimit                  = 10000;

/**
 * Flag to indicate whether kernel is running.
 */
bool_t                              isKernelRunning              = GOS_FALSE;

/**
 * Force idle flag.
 */
bool_t                              forceIdle;

/*
 * Static variables
 */
/**
 * Previous tick value for sleep and block tick calculation.
 */
GOS_STATIC u32_t                    previousTick                 = 0u;

/*
 * External variables
 */
GOS_EXTERN gos_taskDescriptor_t     taskDescriptors [CFG_TASK_MAX_NUMBER];

/*
 * Function prototypes
 */
GOS_STATIC void_t  gos_kernelSelectNextTask     (void_t);
GOS_STATIC char_t* gos_kernelGetTaskStateString (gos_taskState_t taskState);

/*
 * External functions
 */
GOS_EXTERN void_t  gos_idleTask                 (void_t);

/*
 * Function: gos_kernelInit
 */
gos_result_t gos_kernelInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t  initResult = GOS_ERROR;
    u16_t         taskIndex  = 1u;

    /*
     * Function code.
     */        
    taskDescriptors[0].taskFunction       = gos_idleTask;
    taskDescriptors[0].taskId             = GOS_DEFAULT_TASK_ID;
    taskDescriptors[0].taskPriority       = GOS_TASK_IDLE_PRIO;
    taskDescriptors[0].taskState          = GOS_TASK_READY;
    taskDescriptors[0].taskPrivilegeLevel = GOS_TASK_PRIVILEGE_KERNEL;

    forceIdle = GOS_TRUE;

    // Initialize task descriptors.
    for (taskIndex = 1u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
    {
        taskDescriptors[taskIndex].taskFunction         = NULL;
        taskDescriptors[taskIndex].taskPriority         = GOS_TASK_MAX_PRIO_LEVELS;
        taskDescriptors[taskIndex].taskState            = GOS_TASK_SUSPENDED;
        taskDescriptors[taskIndex].taskId               = GOS_INVALID_TASK_ID;
    }

    cont_init(&taskDescriptors[0].taskContext);

    initResult = GOS_SUCCESS;
        
    return initResult;
}

/*
 * Function: gos_kernelStart
 */
gos_result_t gos_kernelStart (void_t)
{
    /*
     * Function code.
     */  
    if (forceIdle == GOS_TRUE)
    {
        currentTaskIndex = 0;
    }
    else
    {
        gos_kernelSelectNextTask();
    }

    // Set kernel running flag.
    isKernelRunning = GOS_TRUE;

    cont_run(&taskDescriptors[currentTaskIndex].taskContext, taskDescriptors[currentTaskIndex].taskFunction);

    return GOS_SUCCESS;
}

#if 0
/*
 * Function: gos_kernelRegisterSysTickHook
 */
gos_result_t gos_kernelRegisterSysTickHook (gos_sysTickHook_t sysTickHookFunction)
{
    /*
     * Local variables.
     */
    gos_result_t hookRegisterResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (sysTickHookFunction != NULL && kernelSysTickHookFunction == NULL)
    {
        kernelSysTickHookFunction = sysTickHookFunction;
        hookRegisterResult = GOS_SUCCESS;
    }
    else
    {
        // Nothing to do.
    }

    return hookRegisterResult;
}

/*
 * Function: gos_ported_sysTickInterrupt
 */
#include <gos_time.h>
void_t gos_ported_sysTickInterrupt (void_t)
{
    /*
     * Function code.
     */
    sysTicks++;
    (void_t) gos_timeIncreaseSystemTime(1);

    // Periodic stack overflow check on currently running task.
    if (isKernelRunning == GOS_TRUE)
    {
        gos_kernelCheckTaskStack();

#if CFG_SCHED_COOPERATIVE == 0
        if (schedDisableCntr == 0u)
        {
            // Privileged.
            gos_kernelReschedule(GOS_PRIVILEGED);
        }
        else
        {
            // Scheduling disabled.
        }
#endif
    }
    else
    {
        // Kernel is not running.
    }

    if (kernelSysTickHookFunction != NULL)
    {
        kernelSysTickHookFunction();
    }
    else
    {
        // Nothing to do.
    }
}
#endif

/*
 * Function: gos_kernelGetSysTicks
 */
u32_t gos_kernelGetSysTicks (void_t)
{
    /*
     * Function code.
     */
    //return sysTicks;
    return millis();
}
#if 0
/*
 * Function: gos_kernelGetCpuUsage
 */
u16_t gos_kernelGetCpuUsage (void_t)
{
    /*
     * Function code.
     */
    return (10000 - taskDescriptors[0].taskCpuMonitoringUsage);
}
#endif

/*
 * Function: gos_kernelDelayMs
 */
GOS_INLINE void_t gos_kernelDelayMs (u16_t milliseconds)
{
    /*
     * Local variables.
     */
    u32_t sysTickVal = millis();

    /*
     * Function code.
     */
    while ((u16_t)(millis() - sysTickVal) < milliseconds);
}
#if 0
/*
 * Function: gos_kernelCalculateTaskCpuUsages
 */
GOS_INLINE void_t gos_kernelCalculateTaskCpuUsages (bool_t isResetRequired)
{
    /*
     * Local variables.
     */
    u16_t taskIndex           = 0u;
    u32_t systemConvertedTime = 0u;
    u32_t taskConvertedTime   = 0u;

    /*
     * Function code.
     */
    // Calculate in microseconds.
    systemConvertedTime = monitoringTime.minutes * 60 * 1000 * 1000 +
                          monitoringTime.seconds * 1000 * 1000 +
                          monitoringTime.milliseconds * 1000 +
                          monitoringTime.microseconds;

    for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
    {
        if (taskDescriptors[taskIndex].taskFunction == NULL)
        {
            break;
        }
        else
        {
            // Continue.
        }

        taskConvertedTime   = taskDescriptors[taskIndex].taskMonitoringRunTime.minutes * 60 * 1000 * 1000 +
                              taskDescriptors[taskIndex].taskMonitoringRunTime.seconds * 1000 * 1000 +
                              taskDescriptors[taskIndex].taskMonitoringRunTime.milliseconds * 1000 +
                              taskDescriptors[taskIndex].taskMonitoringRunTime.microseconds;

        if (systemConvertedTime > 0)
        {
            // Calculate CPU usage and then reset runtime counter.
            taskDescriptors[taskIndex].taskCpuMonitoringUsage = (u16_t)(((u32_t)10000 * taskConvertedTime) / systemConvertedTime);

            if (isResetRequired == GOS_TRUE || monitoringTime.seconds > 0)
            {
                taskDescriptors[taskIndex].taskCpuUsage = (u16_t)((u32_t)(10000 * taskConvertedTime) / systemConvertedTime);

                // Increase runtime microseconds.
                (void_t) gos_runTimeAddMicroseconds(
                        &taskDescriptors[taskIndex].taskRunTime,
                        NULL,
                        (u16_t)taskDescriptors[taskIndex].taskMonitoringRunTime.microseconds);

                // Increase runtime milliseconds.
                (void_t) gos_runTimeAddMilliseconds(
                        &taskDescriptors[taskIndex].taskRunTime,
                        (u32_t)(taskDescriptors[taskIndex].taskMonitoringRunTime.milliseconds +
                        taskDescriptors[taskIndex].taskMonitoringRunTime.seconds * 1000));

                taskDescriptors[taskIndex].taskMonitoringRunTime.days         = 0u;
                taskDescriptors[taskIndex].taskMonitoringRunTime.hours        = 0u;
                taskDescriptors[taskIndex].taskMonitoringRunTime.minutes      = 0u;
                taskDescriptors[taskIndex].taskMonitoringRunTime.seconds      = 0u;
                taskDescriptors[taskIndex].taskMonitoringRunTime.milliseconds = 0u;
                taskDescriptors[taskIndex].taskMonitoringRunTime.microseconds = 0u;

                // Store the highest CPU usage value.
                if (taskDescriptors[taskIndex].taskCpuUsage > taskDescriptors[taskIndex].taskCpuUsageMax)
                {
                    taskDescriptors[taskIndex].taskCpuUsageMax = taskDescriptors[taskIndex].taskCpuUsage;
                }
                else
                {
                    // Max. value has not been reached.
                }
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
    }

    // Reset monitoring time.
    if (isResetRequired == GOS_TRUE || monitoringTime.seconds > 0)
    {
        monitoringTime.days         = 0u;
        monitoringTime.hours        = 0u;
        monitoringTime.minutes      = 0u;
        monitoringTime.seconds      = 0u;
        monitoringTime.milliseconds = 0u;
        monitoringTime.microseconds = 0u;
    }
    else
    {
        // Nothing to do.
    }
}
#endif

/*
 * Function: gos_kernelDump
 */
void_t gos_kernelDump (void_t)
{
    /*
     * Local variables.
     */
    u16_t taskIndex = 0u;

    /*
     * Function code.
     */
    (void_t) gos_shellDriverTransmitString("Task dump:\r\n");
    (void_t) gos_shellDriverTransmitString(TASK_DUMP_SEPARATOR);
    (void_t) gos_shellDriverTransmitString(
        "| %6s | %28s | %4s | %16s | %9s |\r\n",
        "tid",
        "name",
        "prio",
        "privileges",
        "state"
        );
    (void_t) gos_shellDriverTransmitString(TASK_DUMP_SEPARATOR);

    for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
    {
        if (taskDescriptors[taskIndex].taskFunction == NULL)
        {
            break;
        }
        else
        {
            (void_t) gos_shellDriverTransmitString(
                    "| 0x%04X | %28s | %4d | " BINARY_PATTERN " | %13s |\r\n",
                    taskDescriptors[taskIndex].taskId,
                    taskDescriptors[taskIndex].taskName,
                    taskDescriptors[taskIndex].taskPriority,
                    TO_BINARY((u16_t)taskDescriptors[taskIndex].taskPrivilegeLevel),
                    gos_kernelGetTaskStateString(taskDescriptors[taskIndex].taskState)
                    );
        }
    }
    (void_t) gos_shellDriverTransmitString(TASK_DUMP_SEPARATOR"\n");
#if 0
    // Max CPU load statistics.
    (void_t) gos_shellDriverTransmitString("CPU max loads:\r\n");
    (void_t) gos_shellDriverTransmitString(MAX_CPU_DUMP_SEPARATOR);
    (void_t) gos_shellDriverTransmitString(
        "| %6s | %28s | %11s |\r\n",
        "tid",
        "name",
        "max cpu [%]"
        );
    (void_t) gos_shellDriverTransmitString(MAX_CPU_DUMP_SEPARATOR);

    for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
    {
        if (taskDescriptors[taskIndex].taskFunction == NULL)
        {
            break;
        }
        else
        {
            (void_t) gos_shellDriverTransmitString(
                    "| 0x%04X | %28s | %8u.%02u |\r\n",
                    taskDescriptors[taskIndex].taskId,
                    taskDescriptors[taskIndex].taskName,
                    taskDescriptors[taskIndex].taskCpuUsageMax / 100,
                    taskDescriptors[taskIndex].taskCpuUsageMax % 100
                    );
        }
    }
    (void_t) gos_shellDriverTransmitString(MAX_CPU_DUMP_SEPARATOR"\n");
#endif
    // Stack stats.
    #if 0
    (void_t) gos_shellDriverTransmitString("Stack statistics:\r\n");
    (void_t) gos_shellDriverTransmitString(STACK_STATS_SEPARATOR);
    (void_t) gos_shellDriverTransmitString(
        "| %6s | %28s | %6s | %14s | %9s |\r\n",
        "tid",
        "name",
        "stack",
        "max stack use",
        "stack [%]"
        );
    (void_t) gos_shellDriverTransmitString(STACK_STATS_SEPARATOR);
    for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
    {
        if (taskDescriptors[taskIndex].taskFunction == NULL)
        {
            break;
        }
        else
        {
            (void_t) gos_shellDriverTransmitString(
                    "| 0x%04X | %28s | 0x%04X | 0x%-12X | %6u.%02u |%\r\n",
                    taskDescriptors[taskIndex].taskId,
                    taskDescriptors[taskIndex].taskName,
                    taskDescriptors[taskIndex].taskStackSize,
                    taskDescriptors[taskIndex].taskStackSizeMaxUsage,
                    ((10000 * taskDescriptors[taskIndex].taskStackSizeMaxUsage) / taskDescriptors[taskIndex].taskStackSize) / 100,
                    ((10000 * taskDescriptors[taskIndex].taskStackSizeMaxUsage) / taskDescriptors[taskIndex].taskStackSize) % 100
                    );
        }
    }
    (void_t) gos_shellDriverTransmitString(STACK_STATS_SEPARATOR"\n");
    #endif
}

/**
 * @brief   Selects the next task for execution.
 * @details Loops through the internal task descriptor array and first checks
 *          the sleeping tasks and wakes up the ones that passed their sleeping time.
 *          Then based on the priority of the ready tasks, it selects the one with
 *          the highest priority (lowest number in priority). If there is a swap-hook
 *          function registered, it calls it, and then it refreshes the task run-time
 *          statistics.
 *
 * @return    -
 */
GOS_UNUSED GOS_STATIC void_t gos_kernelSelectNextTask (void_t)
{
    /*
     * Local variables.
     */
    u16_t          taskIndex      = 0u;
    gos_taskPrio_t lowestPrio     = GOS_TASK_IDLE_PRIO;
    u16_t          nextTask       = 0u;
    u32_t          elapsedTicks   = millis() - previousTick;

    /*
     * Function code.
     */
    if (schedDisableCntr == 0u)
    {
        for (taskIndex = 0U; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
        {
            // Wake-up sleeping tasks if their sleep time has elapsed.
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_SLEEPING &&
                (taskDescriptors[taskIndex].taskSleepTickCounter += elapsedTicks) >= taskDescriptors[taskIndex].taskSleepTicks)
            {
                taskDescriptors[taskIndex].taskState = GOS_TASK_READY;
            }
            // Unblock tasks if their timeout time has elapsed.
            else if ((taskDescriptors[taskIndex].taskState == GOS_TASK_BLOCKED) &&
                    (taskDescriptors[taskIndex].taskBlockTicks != GOS_TASK_MAX_BLOCK_TIME_MS) &&
                    ((taskDescriptors[taskIndex].taskBlockTickCounter += elapsedTicks) == taskDescriptors[taskIndex].taskBlockTicks))
            {
                taskDescriptors[taskIndex].taskState = GOS_TASK_READY;
            }
            else
            {
                // Nothing to do.
            }

            // Choose the highest priority task - that is not the current one, and is ready - to run.
            // Also, check if task has reached the CPU usage limit.
            if (taskIndex != currentTaskIndex &&
                taskDescriptors[taskIndex].taskState == GOS_TASK_READY &&
                taskDescriptors[taskIndex].taskPriority < lowestPrio)
            {
                nextTask = taskIndex;
                lowestPrio = taskDescriptors[taskIndex].taskPriority;
            }
            else
            {
                // Nothing to do.
            }

            if (taskDescriptors[taskIndex].taskFunction == NULL)
            {
                break;
            }
            else
            {
                // Continue.
            }
        }

        // Set current task.
        currentTaskIndex = nextTask;

        // Update previous tick value.
        previousTick = millis();
    }
    else
    {
        // Nothing to do.
    }
}

/**
 * @brief   Translates the task state to a string.
 * @details Based on the task state it returns a string with a printable form
 *          of the task state.
 *
 * @param   taskState : The task state variable to be translated.
 *
 * @return  String with the task state.
 */
GOS_STATIC char_t* gos_kernelGetTaskStateString (gos_taskState_t taskState)
{
    /*
     * Function code.
     */
    switch (taskState)
    {
        case GOS_TASK_READY:
        {
            return TRACE_FG_GREEN_START"ready    "TRACE_FORMAT_RESET;
        }break;
        case GOS_TASK_SLEEPING:
        {
            return TRACE_FG_YELLOW_START"sleeping "TRACE_FORMAT_RESET;
        }break;
        case GOS_TASK_SUSPENDED:
        {
            return TRACE_FG_MAGENTA_START"suspended"TRACE_FORMAT_RESET;
        }break;
        case GOS_TASK_BLOCKED:
        {
            return TRACE_FG_CYAN_START"blocked  "TRACE_FORMAT_RESET;
        }break;
        case GOS_TASK_ZOMBIE:
        {
            return TRACE_FG_RED_START"zombie   "TRACE_FORMAT_RESET;
        }
        default:
        {
            return "";
        }
    }
}
