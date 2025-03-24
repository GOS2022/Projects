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
//! @file       gos_kernel.h
//! @author     Ahmed Gazar
//! @date       2023-11-01
//! @version    1.19
//!
//! @brief      GOS kernel header.
//! @details    The GOS kernel is the core of the GOS system. It contains the basic type
//!             definitions of the system (these are used across the OS in other services), and it
//!             handles the system tick, tasks, and scheduling. The kernel module also provides an
//!             interface for registering tasks, changing their states, registering hook functions,
//!             disabling the scheduler. It also provides a service to dump the kernel
//!             configuration and task information on the log output.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2022-10-21    Ahmed Gazar     Initial version created.
// 1.1        2022-11-14    Ahmed Gazar     +    Task previous state added to task descriptor type
//                                          +    Dump signal sender enumerators added
// 1.2        2022-11-15    Ahmed Gazar     +    License added
// 1.3        2022-12-03    Ahmed Gazar     +    gos_kernelTaskRegisterTasks added
//                                          +    GOS_PARAM_IGNORE macro added
//                                          +    taskIdEx added to gos_taskDescriptor_t
// 1.4        2022-12-04    Ahmed Gazar     *    Sleep, wake-up, suspend, resume, block, unblock,
//                                               delete hook functions replaced with signals
// 1.5        2022-12-08    Ahmed Gazar     +    Task original priority added
//                                          +    Task priority setter and getter functions added
// 1.6        2022-12-11    Ahmed Gazar     -    GOS_PARAM_IGNORE removed
//                                          +    Interface descriptions added
// 1.7        2022-12-15    Ahmed Gazar     +    Task privilege levels and privilege control macros
//                                               and functions added
// 1.8        2023-01-13    Ahmed Gazar     +    Constant macro added
//                                          *    Scheduling disabled flag replaced with counter
// 1.9        2023-05-04    Ahmed Gazar     +    Task stack monitoring variables added
// 1.10       2023-05-19    Ahmed Gazar     +    gos_kernelGetCpuUsage added
// 1.11       2023-06-17    Ahmed Gazar     *    Kernel dump moved to function
// 1.12       2023-06-18    Ahmed Gazar     -    Dump signal sender IDs removed
// 1.13       2023-06-30    Ahmed Gazar     *    gos_kernelCalculateTaskCpuUsages reset required
//                                               flag added
//                                          +    gos_kernelSetMaxCpuLoad added
//                                          +    gos_kernelGetMaxCpuLoad added
//                                          +    gos_kernelIsCallerIsr added
//                                          +    gos_kernelIsCallerPrivileged added
//                                          +    gos_kernelTaskSetPrivileges  added
// 1.14       2023-07-11    Ahmed Gazar     -    GOS_PRIVILEGED_ACCESS removed
//                                          -    GOS_UNPRIVILEGED_ACCESS removed
//                                          +    GOS_TASK_MAX_BLOCK_TIME_MS added
//                                          -    GOS_IS_ACCESS_PRIVILEGED() removed
//                                          -    GOS_PRIV_MODIFY removed
//                                          +    gos_blockMaxTick_t added
//                                          +    gos_sysTickHook_t added
//                                          +    taskBlockMaxTicks added to gos_taskDescriptor_t
//                                          +    gos_kernelTaskGetPrivileges added
//                                          +    gos_kernelRegisterSysTickHook added
//                                          -    GOS_IS_CALLER_ISR() removed
// 1.15       2023-07-25    Ahmed Gazar     +    taskMonitoringRunTime added to task descriptor
//                                          +    gos_kernelTaskGetDataByIndex
// 1.16       2023-09-08    Ahmed Gazar     +    Task sleep tick and block tick counter added to
//                                               task descriptor structure
// 1.17       2023-09-25    Ahmed Gazar     *    Task stack variables modified
// 1.18       2023-10-20    Ahmed Gazar     +    gos_privilegedHook_t added
//                                          +    gos_kernelRegisterPrivilegedHook added
//                                          +    gos_kernelPrivilegedModeSetRequired added
// 1.19       2023-11-01    Ahmed Gazar     *    RAM_START, RAM_SIZE, GLOBAL_STACK, and MAIN_STACK
//                                               macros moved here
//                                          *    GOS_ATOMIC_ENTER and GOS_ATOMIC_EXIT modified
//                                          *    gos_kernel_privilege_t moved here
//                                          -    taskIdEx removed from task descriptor structure
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
#ifndef GOS_KERNEL_H
#define GOS_KERNEL_H
/*
 * Includes
 */
#include <stdint.h>

/*
 * Macros
 */
#ifndef NULL
/**
 * NULL pointer.
 */
#define NULL                           ( (void *) 0 )
#endif
/**
 * Static macro.
 */
#define GOS_STATIC                     static

/**
 * Constant macro.
 */
#define GOS_CONST                      const

/**
 * Inline macro.
 */
#define GOS_INLINE                     inline __attribute__((always_inline))

/**
 * Static in-line macro.
 */
#define GOS_STATIC_INLINE              GOS_STATIC GOS_INLINE

/**
 * Extern macro.
 */
#define GOS_EXTERN                     extern

/**
 * Naked.
 */
#define GOS_NAKED                      __attribute__ ((naked))

/**
 * Unused.
 */
#define GOS_UNUSED                     __attribute__ ((unused))

/**
 * NOP.
 */
#define GOS_NOP                        __asm volatile ("NOP")

/**
 * ASM.
 */
#define GOS_ASM                        __asm volatile

/**
 * Disable scheduling.
 */
#define GOS_DISABLE_SCHED              {                                            \
                                           GOS_EXTERN u8_t schedDisableCntr;        \
                                           schedDisableCntr++;                      \
                                       }
/**
 * Enable scheduling.
 */
#define GOS_ENABLE_SCHED               {                                            \
                                           GOS_EXTERN u8_t schedDisableCntr;        \
                                           if (schedDisableCntr > 0)                \
                                           { schedDisableCntr--; }                  \
                                       }

/*
 * Type definitions
 */

/*
 * Basic data types.
 */
typedef uint8_t  bool_t;                                       //!< Boolean logic type.
typedef uint8_t  u8_t;                                         //!< 8-bit unsigned type.
typedef uint16_t u16_t;                                        //!< 16-bit unsigned type.
typedef unsigned long u32_t;                                        //!< 32-bit unsigned type.
typedef uint64_t u64_t;                                        //!< 64-bit unsigned type.
typedef int8_t   s8_t;                                         //!< 8-bit signed type.
typedef int16_t  s16_t;                                        //!< 16-bit signed type.
typedef signed long  s32_t;                                        //!< 32-bit signed type.
typedef int64_t  s64_t;                                        //!< 64-bit signed type.
typedef char     char_t;                                       //!< 8-bit character type.
typedef float    float_t;                                      //!< Single precision float type.
typedef double   double_t;                                     //!< Double precision float type.
typedef void     void_t;                                       //!< Void type.

/**
 *  Result type enumerator.
 *  @note Hamming distance of
 *  - success and error:    7
 *  - success and busy:     4
 *  - error and busy:       5
 */
typedef enum
{
    GOS_SUCCESS = 0b01010101, //!< Success.
    GOS_ERROR   = 0b10101110, //!< Error.
    GOS_BUSY    = 0b10110001  //!< Busy.
}gos_result_t;

/**
 *  Boolean values.
 *  @note Hamming distance of true and false value: 7.
 */
typedef enum
{
    GOS_TRUE  = 0b00110110,   //!< True value.
    GOS_FALSE = 0b01001001    //!< False value.
}gos_boolValue_t;

typedef u16_t   gos_microsecond_t; //!< Microsecond type.
typedef u16_t   gos_millisecond_t; //!< Millisecond type.
typedef u8_t    gos_second_t;      //!< Second type.
typedef u8_t    gos_minute_t;      //!< Minute type.
typedef u8_t    gos_hour_t;        //!< Hour type.
typedef u16_t   gos_day_t;         //!< Day type.
typedef u8_t    gos_month_t;       //!< Month type.
typedef u16_t   gos_year_t;        //!< Year type.

/**
 * Run-time type.
 */
typedef struct __attribute__((packed))
{
    gos_microsecond_t microseconds; //!< Microseconds.
    gos_millisecond_t milliseconds; //!< Milliseconds.
    gos_second_t      seconds;      //!< Seconds.
    gos_minute_t      minutes;      //!< Minutes.
    gos_hour_t        hours;        //!< Hours.
    gos_day_t         days;         //!< Days.
}gos_runtime_t;

/**
 *  Task state enumerator.
 *
 *  @note Hamming distance of
 *  - ready and sleeping:       3
 *  - ready and blocked:        3
 *  - ready and suspended:      4
 *  - ready and zombie:         3
 *  - sleeping and blocked:     4
 *  - sleeping and suspended:   3
 *  - sleeping and zombie:      4
 *  - blocked and suspended:    3
 *  - blocked and zombie:       2
 *  - suspended and zombie:     1
 */
typedef enum
{
    GOS_TASK_READY      = 0b01010,   //!< Task is ready to be scheduled.
    GOS_TASK_SLEEPING   = 0b10110,   //!< Task is sleeping (waiting for wake-up ticks).
    GOS_TASK_BLOCKED    = 0b11001,   //!< Task is blocked (waiting for resource).
    GOS_TASK_SUSPENDED  = 0b00101,   //!< Task is suspended (not to be scheduled), but can be resumed.
    GOS_TASK_ZOMBIE     = 0b01101    //!< Task deleted (physically existing in memory, but cannot be resumed).
}gos_taskState_t;

/**
 * Task privilege level enumerator.
 */
typedef enum
{
    GOS_TASK_PRIVILEGE_SUPERVISOR = 0xFFFF,    //!< Task supervisor privilege level.
    GOS_TASK_PRIVILEGE_KERNEL     = 0xFF00,    //!< Task kernel privilege level.
    GOS_TASK_PRIVILEGE_USER       = 0x00FF,    //!< Task user privilege level.
    GOS_TASK_PRIVILEGED_USER      = 0x20FF     //!< User with logging right.
}gos_taskPrivilegeLevel_t;

gos_result_t gos_kernelInit (void_t);

gos_result_t gos_kernelStart (void_t);

u32_t gos_kernelGetSysTicks (void_t);

void_t gos_kernelDelayMs (u16_t milliseconds);

void_t gos_kernelDump (void_t);

#endif
