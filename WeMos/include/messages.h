#ifndef MESSAGES_H
#define MESSAGES_H

#include "gos.h"

/*#define MSG_LED_TEST_ID       ( 0x1001 )
#define MSG_SYSTEM_RESET_ID   ( 0x100A )
#define MSG_CPU_LOAD_REQ_ID   ( 0x100B )
#define MSG_CPU_LOAD_RESP_ID  ( 0x600B )
#define MSG_TASK_NUM_REQ_ID   ( 0x100C )
#define MSG_TASK_NUM_RESP_ID  ( 0x600C )
#define MSG_TASK_DATA_REQ_ID  ( 0x100D )
#define MSG_TASK_DATA_RESP_ID ( 0x600D )
#define MSG_PING_REQ_ID       ( 0x100E )
#define MSG_PING_RESP_ID      ( 0x600E )
#define MSG_RUNTIME_REQ_ID    ( 0x100F )
#define MSG_RUNTIME_RESP_ID   ( 0x600F )
#define MSG_SWINFO_REQ_ID     ( 0x1010 )
#define MSG_SWINFO_RESP_ID    ( 0x6010 )
#define MSG_TASK_VAR_DATA_REQ_ID  ( 0x1011 )
#define MSG_TASK_VAR_DATA_RESP_ID ( 0x6011 )
#define MSG_TASK_MODIFY_REQ_ID (0x1012)
#define MSG_TASK_MODIFY_RESP_ID (0x6012)
#define MSG_HWINFO_REQ_ID     ( 0x1013 )
#define MSG_HWINFO_RESP_ID    ( 0x6013 )*/

typedef struct
{
	u8_t pingResult;
}svl_ipcPingMsg_t;

typedef struct
{
	gos_runtime_t sysRunTime;
}svl_ipcRunTimeMsg_t;

/**
 * Library name length.
 */
#define LIB_NAME_LENGTH        ( 48 )

/**
 * Library description length.
 */
#define LIB_DESCRIPTION_LENGTH ( 64 )

/**
 * Library author length.
 */
#define LIB_AUTHOR_LENGTH      ( 32 )

typedef struct __attribute__((packed))
{
	u16_t 	   major;
	u16_t	   minor;
	u16_t 	   build;
	gos_time_t date;
	char_t     name        [LIB_NAME_LENGTH];
	char_t     description [LIB_DESCRIPTION_LENGTH];
	char_t     author      [LIB_AUTHOR_LENGTH];
}gos_libVersion_t;

int x = sizeof(gos_libVersion_t);

typedef struct __attribute__((packed))
{
	u32_t            initPattern;
	u32_t            startAddress;
	u32_t            size;
	u32_t            crc        __attribute__((aligned(8)));
	gos_libVersion_t libVersion __attribute__((aligned(8)));
	gos_libVersion_t version    __attribute__((aligned(8)));
	bool_t           updateMode;
	u32_t            dataCrc    __attribute__((aligned(8)));
}bld_bootloaderData_t;

int y = sizeof(bld_bootloaderData_t);

typedef struct __attribute__((packed))
{
	u32_t            initPattern;
	u32_t            startAddress;
	u32_t            size;
	u32_t            crc;
	gos_libVersion_t libVersion;
	gos_libVersion_t version;
	u32_t            dataCrc __attribute__((aligned(8)));
}bld_appData_t;

int z = sizeof(bld_appData_t);

typedef struct
{
    u8_t ledState;
}ledTestMessage_t;

typedef struct
{
	u16_t cpuLoad;
}svl_ipcCpuLoadMsg_t;

typedef struct
{
	u16_t taskNumber;
}svl_ipcTaskNumberMsg_t;

typedef struct
{
	u16_t taskIdx;
}svl_ipcTaskDataGetMsg_t;

// Special definitions.
/**
 * Maximum task name length.
 */
#define CFG_TASK_MAX_NAME_LENGTH        ( 32 )
typedef char_t   gos_taskName_t [CFG_TASK_MAX_NAME_LENGTH];    //!< Task name type.
typedef u32_t    gos_taskCSCounter_t;                          //!< Context-switch counter type.
typedef u16_t    gos_taskStackSize_t;                          //!< Task stack size type.

/**
 * Task data message structure.
 */
typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskPrio_t           taskOriginalPriority;       //!< Task original priority.
    gos_taskPrivilegeLevel_t taskPrivilegeLevel;         //!< Task privilege level.
    gos_taskName_t           taskName;                   //!< Task name.
    gos_tid_t                taskId;                     //!< Task ID (internal).
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_taskStackSize_t      taskStackSize;              //!< Task stack size.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageLimit;          //!< Task CPU usage limit in [% x 100].
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task max. stack usage.
}svl_ipcTaskDataMsg_t;

typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task stack usage.
}svl_ipcTaskVariableDataMsg_t;

#endif