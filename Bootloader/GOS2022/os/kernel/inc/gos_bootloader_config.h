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
//! @file       gos_bootloader_config.h
//! @author     Ahmed Gazar
//! @date       2023-09-26
//! @version    1.0
//!
//! @brief      GOS bootloader configuration header.
//! @details    This header contains the kernel and service configurations of the operating system.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2023-09-26    Ahmed Gazar     Initial version created.
//*************************************************************************************************
#ifndef GOS_BOOTLOADER_CONFIG_H
#define GOS_BOOTLOADER_CONFIG_H
/*
 * Includes
 */
#include <stdint.h>

/*
 * Macros
 */
/*
 * Supported target CPU list.
 */
/**
 * ARM Cortex-M4.
 */
#define ARM_CORTEX_M4                   ( 1 )

/**
 * Target CPU.
 */
#define CFG_TARGET_CPU                  ( ARM_CORTEX_M4 )

/*
 * Scheduling and priority inheritance.
 */
/**
 * Cooperative scheduling flag.
 */
#define CFG_SCHED_COOPERATIVE           ( 0 )
/**
 * Priority inheritance flag for lock.
 */
#define CFG_USE_PRIO_INHERITANCE        ( 1 )

/*
 * General task parameters.
 */
/**
 * Maximum task name length.
 */
#define CFG_TASK_MAX_NAME_LENGTH        ( 32 )
/**
 * Maximum number of tasks.
 */
#define CFG_TASK_MAX_NUMBER             ( 16 )

/*
 * OS task stack sizes.
 */
/**
 * Minimum task stack size.
 */
#define CFG_TASK_MIN_STACK_SIZE         ( 0x200 )
/**
 * Maximum task stack size.
 */
#define CFG_TASK_MAX_STACK_SIZE         ( 0x4000 )
/**
 * Idle task stack size.
 */
#define CFG_IDLE_TASK_STACK_SIZE        ( 0x400 )
/**
 * System task stack size.
 */
#define CFG_SYSTEM_TASK_STACK_SIZE      ( 0x1000 )
/**
 * Signal daemon task stack size.
 */
#define CFG_TASK_SIGNAL_DAEMON_STACK    ( 0x400 )
/**
 * Time daemon task stack size.
 */
#define CFG_TASK_TIME_DAEMON_STACK      ( 0x300 )
/**
 * Message daemon task stack size.
 */
#define CFG_TASK_MESSAGE_DAEMON_STACK   ( 0x300 )
/**
 * Shell daemon task stack size.
 */
#define CFG_TASK_SHELL_DAEMON_STACK     ( 0x300 )
/**
 * Log daemon task stack size.
 */
#define CFG_TASK_TRACE_DAEMON_STACK     ( 0x400 )

/*
 * OS task priorities.
 */
/**
 * Trace daemon task priority.
 */
#define CFG_TASK_TRACE_DAEMON_PRIO      ( 193 )
/**
 * Message daemon task priority.
 */
#define CFG_TASK_MESSAGE_DAEMON_PRIO    ( 198 )
/**
 * Signal daemon task priority.
 */
#define CFG_TASK_SIGNAL_DAEMON_PRIO     ( 197 )
/**
 * Shell daemon task priority.
 */
#define CFG_TASK_SHELL_DAEMON_PRIO      ( 192 )
/**
 * Time daemon task priority.
 */
#define CFG_TASK_TIME_DAEMON_PRIO       ( 196 )
/**
 * System task priority.
 */
#define CFG_TASK_SYS_PRIO               ( 195 )

/*
 * Queue service parameters.
 */
/**
 * Maximum number of queues.
 */
#define CFG_QUEUE_MAX_NUMBER            ( 1 )
/**
 * Maximum number of queue elements.
 */
#define CFG_QUEUE_MAX_ELEMENTS          ( 30 )
/**
 * Maximum queue length.
 */
#define CFG_QUEUE_MAX_LENGTH            ( 200 )
/**
 * Queue use name flag.
 */
#define CFG_QUEUE_USE_NAME              ( 1 )
/**
 * Maximum queue name length.
 */
#define CFG_QUEUE_MAX_NAME_LENGTH       ( 24 )


/*
 * Signal service parameters.
 */
/**
 * Maximum number of signals.
 */
#define CFG_SIGNAL_MAX_NUMBER           ( 3 )
/**
 * Maximum number of signal subscribers.
 */
#define CFG_SIGNAL_MAX_SUBSCRIBERS      ( 6 )

/*
 * Message service parameters.
 */
/**
 * Maximum number of messages handled at once.
 */
#define CFG_MESSAGE_MAX_NUMBER          ( 16 )
/**
 * Maximum length of a message in bytes.
 */
#define CFG_MESSAGE_MAX_LENGTH          ( 80 )
/**
 * Maximum number of message waiters.
 */
#define CFG_MESSAGE_MAX_WAITERS         ( 16 )
/**
 * Maximum number of message IDs a task can wait for (includes the terminating 0).
 */
#define CFG_MESSAGE_MAX_WAITER_IDS      ( 8 )

/*
 * Shell service parameters.
 */
/**
 * Shell service use flag.
 */
#define CFG_SHELL_USE_SERVICE           ( 0 )
/**
 * Maximum number of shell commands.
 */
#define CFG_SHELL_MAX_COMMAND_NUMBER    ( 8 )
/**
 * Maximum command length.
 */
#define CFG_SHELL_MAX_COMMAND_LENGTH    ( 20 )
/**
 * Maximum parameters length.
 */
#define CFG_SHELL_MAX_PARAMS_LENGTH     ( 64 )
/**
 * Command buffer size.
 */
#define CFG_SHELL_COMMAND_BUFFER_SIZE   ( 200 )
/**
 * Shell daemon startup delay time [ms].
 */
#define CFG_SHELL_STARTUP_DELAY_MS      ( 500 )

/*
 * GCP service parameters.
 */
/**
 * GCP maximum number of channels.
 */
#define CFG_GCP_CHANNELS_MAX_NUMBER     ( 2 )

/*
 * Trace service parameters.
 */
/**
 * Trace maximum (line) length.
 */
#define CFG_TRACE_MAX_LENGTH            ( 200 )

/*
 * Error handling parameters.
 */
/**
 * Flag to indicate if the system should reset on error.
 */
#define CFG_RESET_ON_ERROR              ( 1 )
/**
 * Delay time before system reset.
 */
#define CFG_RESET_ON_ERROR_DELAY_MS     ( 2000 )

#endif
