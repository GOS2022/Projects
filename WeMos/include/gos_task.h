#ifndef GOS_LIGHT_H
#define GOS_LIGHT_H

#include "gos_kernel.h"
#include <cont.h>

#define CFG_TASK_MAX_NUMBER            ( 4u )
#define CFG_TASK_MAX_NAME_LENGTH       ( 32u )
/**
 * Default task ID.
 */
#define GOS_DEFAULT_TASK_ID            ( (gos_tid_t)0x8000 )
/**
 * Invalid task ID.
 */
#define GOS_INVALID_TASK_ID            ( (gos_tid_t)0x0100 )
/**
 * Maximum task priority levels.
 */
#define GOS_TASK_MAX_PRIO_LEVELS       ( UINT8_MAX )

/**
 * Idle task priority.
 */
#define GOS_TASK_IDLE_PRIO             ( (gos_taskPrio_t)GOS_TASK_MAX_PRIO_LEVELS )

/**
 * Task manipulation privilege flag.
 */
#define GOS_PRIV_TASK_MANIPULATE       ( 1 << 15 )

/**
 * Task priority change privilege flag.
 */
#define GOS_PRIV_TASK_PRIO_CHANGE      ( 1 << 14 )

/**
 * Tracing privilege flag.
 */
#define GOS_PRIV_TRACE                 ( 1 << 13 )

/**
 * Task signal invoking privilege flag.
 */
#define GOS_PRIV_SIGNALING             ( 1 << 11 )

/**
 * Kernel reserved.
 */
#define GOS_PRIV_RESERVED_3            ( 1 << 10 )

/**
 * Kernel reserved.
 */
#define GOS_PRIV_RESERVED_4            ( 1 << 9 )

/**
 * Kernel reserved.
 */
#define GOS_PRIV_RESERVED_5            ( 1 << 8 )

/**
 * Task maximum block time.
 */
#define GOS_TASK_MAX_BLOCK_TIME_MS     ( 0xFFFFFFFFu )

typedef u16_t    gos_tid_t;                                    //!< Task ID type.
typedef char_t   gos_taskName_t [CFG_TASK_MAX_NAME_LENGTH];    //!< Task name type.
typedef void_t   (*gos_task_t)(void_t);                        //!< Task function type.
typedef u8_t     gos_taskPrio_t;                               //!< Task priority type.
typedef u32_t    gos_taskSleepTick_t;                          //!< Sleep tick type.
typedef u32_t    gos_blockMaxTick_t;                           //!< Block max. tick type.

/**
 * Task descriptor structure.
 */
typedef struct
{
    gos_task_t               taskFunction;               //!< Task function.
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskState_t          taskPreviousState;          //!< Task previous state (for restoration).
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskPrio_t           taskOriginalPriority;       //!< Task original priority.
    gos_taskPrivilegeLevel_t taskPrivilegeLevel;         //!< Task privilege level.
    gos_taskName_t           taskName;                   //!< Task name.
    gos_tid_t                taskId;                     //!< Task ID (internal).
    gos_taskSleepTick_t      taskSleepTicks;             //!< Task sleep ticks.
    gos_taskSleepTick_t      taskSleepTickCounter;       //!< Task sleep tick counter.
    gos_blockMaxTick_t       taskBlockTicks;             //!< Task block ticks.
    gos_blockMaxTick_t       taskBlockTickCounter;       //!< Task block tick counter.
    cont_t                   taskContext;                //!< Task context.
}gos_taskDescriptor_t;

/**
 * @brief   This function registers an array of tasks for scheduling.
 * @details Checks the task descriptor array pointer and registers the tasks one by one.
 *
 * @param   taskDescriptors : Pointer to a task descriptor structure array.
 * @param   arraySize       : Size of the array in bytes.
 *
 * @return  Result of task registration.
 *
 * @retval  GOS_SUCCESS     : Tasks registered successfully.
 * @retval  GOS_ERROR       : Invalid task descriptor (NULL function pointer,
 *                            invalid priority level, invalid stack size, idle task registration,
 *                            or stack size is not 4-byte-aligned) in one of the array elements or
 *                            task array is full.
 */
gos_result_t gos_taskRegisterTasks (
        gos_taskDescriptor_t* taskDescriptors,
        u16_t                 arraySize
        );

/**
 * @brief   This function registers a task for scheduling.
 * @details Checks the task descriptor parameters and then tries to find the next empty
 *          slot in the internal task array. When it is found, it registers the task in
 *          that slot.
 *
 * @param   taskDescriptor : Pointer to a task descriptor structure.
 * @param   taskId         : Pointer to a variable to hold to assigned task ID value.
 *
 * @return  Result of task registration.
 *
 * @retval  GOS_SUCCESS    : Task registered successfully.
 * @retval  GOS_ERROR      : Invalid task descriptor (NULL function pointer,
 *                           invalid priority level, invalid stack size, idle task registration,
 *                           or stack size is not 4-byte-aligned) or task array is full.
 */
gos_result_t gos_taskRegister (
        gos_taskDescriptor_t* taskDescriptor,
        gos_tid_t*            taskId
        );

/**
 * @brief   Sends the current task to sleeping state.
 * @details Checks the current task and its state, modifies it to sleeping, and
 *          if there is a sleep hook function registered, it calls it. Then, it
 *          invokes a rescheduling.
 *
 * @param   sleepTicks  : Minimum number of ticks until the task should remain in sleeping state.
 *
 * @return  Result of task sleeping.
 *
 * @retval  GOS_SUCCESS : Task successfully sent to sleeping state.
 * @retval  GOS_ERROR   : Function called from idle task or task state is not ready.
 */
gos_result_t gos_taskSleep (
        gos_taskSleepTick_t sleepTicks
        );

/**
 * @brief   Wakes up the given task.
 * @details Checks the current task and its state, modifies it to ready, and
 *          if there is a wake-up hook function registered, it calls it.
 *
 * @param   taskId      : ID of the task to be waken up.
 *
 * @return  Result of task wake-up.
 *
 * @retval  GOS_SUCCESS : Task waken up successfully.
 * @retval  GOS_ERROR   : Task ID is invalid, or task is not sleeping.
 */
gos_result_t gos_taskWakeup (
        gos_tid_t taskId
        );

/**
 * @brief   Sends the given task to suspended state.
 * @details Checks the given task ID and its state, modified it to suspended, and
 *          if there is a suspend hook function registered, it calls it. If the
 *          suspended function is the currently running one, it invokes a rescheduling.
 *
 * @param   taskId     : ID of the task to be suspended.
 *
 * @return  Result of task suspension.
 *
 * @retval  GOS_SUCESS : Task suspended successfully.
 * @retval  GOS_ERROR  : Task ID is invalid, or task state is not ready or sleeping.
 */
gos_result_t gos_taskSuspend (
        gos_tid_t taskId
        );

/**
 * @brief   Resumes the given task.
 * @details Checks the given task ID and its state, modified it to ready, and
 *          if there is a resume hook function registered, it calls it.
 *
 * @param   taskId     : ID of the task to be resumed.
 *
 * @return  Result of task resumption.
 *
 * @retval  GOS_SUCESS : Task resumed successfully.
 * @retval  GOS_ERROR  : Task ID is invalid, or task is not suspended.
 */
gos_result_t gos_taskResume (
        gos_tid_t taskId
        );

/**
 * @brief   Sends the given task to blocked state.
 * @details Checks the given task ID and its state, modified it to blocked, and
 *          if there is a block hook function registered, it calls it. If the blocked
 *          function is the currently running one, it invokes a rescheduling.
 *
 * @param   taskId     : ID of the task to be blocked.
 *
 * @return  Result of task blocking.
 *
 * @retval  GOS_SUCESS : Task blocked successfully.
 * @retval  GOS_ERROR  : Task ID is invalid, or task state is not ready.
 */
gos_result_t gos_taskBlock (
        gos_tid_t          taskId,
        gos_blockMaxTick_t blockTicks
        );

/**
 * @brief   Unblocks the given task.
 * @details Checks the given task ID and its state, modified it to ready, and
 *          if there is an unblock hook function registered, it calls it.
 *
 * @param   taskId     : ID of the task to be unblocked.
 *
 * @return  Result of task unblocking.
 *
 * @retval  GOS_SUCESS : Task unblocked successfully.
 * @retval  GOS_ERROR  : Task ID is invalid, or task is not blocked.
 */
gos_result_t gos_taskUnblock (
        gos_tid_t taskId
        );

/**
 * @brief   Deletes the given task from the scheduling array.
 * @details Checks the given task ID and its state, modifies it to zombie, and
 *          if there is a delete hook function registered, it calls it.
 *
 * @param   taskId      : ID of the task to be deleted.
 *
 * @return  Result of deletion.
 *
 * @retval  GOS_SUCCESS : Task deleted successfully.
 * @retval  GOS_ERROR   : Task is already a zombie.
 */
gos_result_t gos_taskDelete (
        gos_tid_t taskId
        );

/**
 * @brief   Sets the current priority of the given task to the given value (for temporary change).
 * @details Checks the given parameters and sets the current priority of the given task.
 *
 * @param   taskId       : ID of the task to change the priority of.
 * @param   taskPriority : The desired task priority.
 *
 * @return  Result of priority change.
 *
 * @retval  GOS_SUCCESS  : Current priority changed successfully.
 * @retval  GOS_ERROR    : Invalid task ID or priority.
 */
gos_result_t gos_taskSetPriority (
        gos_tid_t      taskId,
        gos_taskPrio_t taskPriority
        );

/**
 * @brief   Sets the original priority of the given task to the given value (for permanent change).
 * @details Checks the given parameters and sets the original priority of the given task.
 *
 * @param   taskId       : ID of the task to change the priority of.
 * @param   taskPriority : The desired task priority.
 *
 * @return  Result of priority change.
 *
 * @retval  GOS_SUCCESS  : Original priority changed successfully.
 * @retval  GOS_ERROR    : Invalid task ID or priority.
 */
gos_result_t gos_taskSetOriginalPriority (
        gos_tid_t      taskId,
        gos_taskPrio_t taskPriority
        );

/**
 * @brief   Gets the current priority of the given task.
 * @details Checks the given parameters and saves the current priority in the given variable.
 *
 * @param   taskId       : ID of the task to get the priority of.
 * @param   taskPriority : Pointer to a priority variable to store the priority in.
 *
 * @return  Result of priority getting.
 *
 * @retval  GOS_SUCCESS  : Current priority getting successfully.
 * @retval  GOS_ERROR    : Invalid task ID or priority variable is NULL.
 */
gos_result_t gos_taskGetPriority (
        gos_tid_t       taskId,
        gos_taskPrio_t* taskPriority
        );

/**
 * @brief   Gets the original priority of the given task.
 * @details Checks the given parameters and saves the original priority in the given variable.
 *
 * @param   taskId       : ID of the task to get the priority of.
 * @param   taskPriority : Pointer to a priority variable to store the priority in.
 *
 * @return  Result of priority getting.
 *
 * @retval  GOS_SUCCESS  : Original priority getting successfully.
 * @retval  GOS_ERROR    : Invalid task ID or priority variable is NULL.
 */
gos_result_t gos_taskGetOriginalPriority (
        gos_tid_t       taskId,
        gos_taskPrio_t* taskPriority
        );

/**
 * @brief   Adds the given privileges to the given task.
 * @details Checks the caller task if it has the privilege to modify task privileges and
 *          if so, it adds the given privileges to the given task.
 *
 * @param   taskId      : ID of the task to give the privileges to.
 * @param   privileges  : Privileges to be added.
 *
 * @return  Result of privilege adding.
 *
 * @retval  GOS_SUCCESS : Privileges added successfully.
 * @retval  GOS_ERROR   : Invalid task ID or caller does not have the privilege to modify task
 *                        privileges.
 */
gos_result_t gos_taskAddPrivilege (
        gos_tid_t                taskId,
        gos_taskPrivilegeLevel_t privileges
        );

/**
 * @brief   Removes the given privileges from the given task.
 * @details Checks the caller task if it has the privilege to modify task privileges and
 *          if so, it removes the given privileges from the given task.
 *
 * @param   taskId      : ID of the task to remove the privileges from.
 * @param   privileges  : Privileges to be removed.
 *
 * @return  Result of privilege removing.
 *
 * @retval  GOS_SUCCESS : Privileges removed successfully.
 * @retval  GOS_ERROR   : Invalid task ID or caller does not have the privilege to modify task
 *                        privileges.
 */
gos_result_t gos_taskRemovePrivilege (
        gos_tid_t                taskId,
        gos_taskPrivilegeLevel_t privileges
        );

/**
 * @brief   Sets the given privileges for the given task.
 * @details Checks the caller task if it has the privilege to modify task privileges and
 *          if so, it sets the given privileges for the given task.
 *
 * @param   taskId      : ID of the task to set the privileges for.
 * @param   privileges  : Privileges to be set.
 *
 * @return  Result of privilege setting.
 *
 * @retval  GOS_SUCCESS : Privileges set successfully.
 * @retval  GOS_ERROR   : Invalid task ID or caller does not have the privilege to modify task
 *                        privileges.
 */
gos_result_t gos_taskSetPrivileges (
        gos_tid_t                taskId,
        gos_taskPrivilegeLevel_t privileges
        );

/**
 * @brief   Gets the privileges of the given task.
 * @details Returns the privilege flags of the given task.
 *
 * @param   taskId      : ID of the task to get the privileges of.
 * @param   privileges  : Variable to store the privilege flags.
 *
 * @return  Result of privilege getting.
 *
 * @retval  GOS_SUCCESS : Privileges get successful.
 * @retval  GOS_ERROR   : Invalid task ID or privilege variable is NULL pointer.
 */
gos_result_t gos_taskGetPrivileges (
        gos_tid_t                 taskId,
        gos_taskPrivilegeLevel_t* privileges
        );

/**
 * @brief   Gets the task name of the task with the given ID.
 * @details Copies the task name corresponding with the given task ID to the task name variable.
 *
 * @param   taskId      : Pointer to a task ID variable to store the returned ID.
 * @param   taskName    : Task name pointer to store the returned task name.
 *
 * @return  Success of task name get.
 *
 * @retval  GOS_SUCCESS : Task name found successfully.
 * @retval  GOS_ERROR   : Invalid task ID or task name variable is NULL.
 */
gos_result_t gos_taskGetName (
        gos_tid_t      taskId,
        gos_taskName_t taskName
        );

/**
 * @brief   Gets the task ID of the task with the given name.
 * @details This function loops through the internal task array and tries to find the
 *          given task name to get the corresponding task ID.
 *
 * @param   taskName    : Name of the task (string).
 * @param   taskId      : Pointer to a task ID variable to store the returned ID.
 *
 * @return  Success of task ID get.
 *
 * @retval  GOS_SUCCESS : Task ID found successfully.
 * @retval  GOS_ERROR   : Task name not found.
 */
gos_result_t gos_taskGetId (
        gos_taskName_t taskName,
        gos_tid_t*     taskId
        );

/**
 * @brief   Returns the ID of the currently running task.
 * @details Returns the ID of the currently running task.
 *
 * @param   taskId      : Pointer to a task ID variable to store the current task ID.
 *
 * @return  Result of current task ID get.
 *
 * @retval  GOS_SUCCESS : Current task ID returned successfully.
 * @retval  GOS_ERROR   : Task ID pointer is NULL.
 */
gos_result_t gos_taskGetCurrentId (
        gos_tid_t* taskId
        );

/**
 * @brief   Returns the task data of the given task.
 * @details Based on the task ID, it copies the content of the internal task
 *          descriptor array element to the given task descriptor.
 *
 * @param   taskId      : ID of the task to get the data of.
 * @param   taskData    : Pointer to the task descriptor to save the task data in.
 *
 * @return  Result of task data get.
 *
 * @retval  GOS_SUCCESS : Task data copied successfully.
 * @retval  GOS_ERROR   : Invalid task ID or task data pointer is NULL.
 */
gos_result_t gos_taskGetData (
        gos_tid_t             taskId,
        gos_taskDescriptor_t* taskData
        );

/**
 * @brief   Returns the task data of the given task.
 * @details Based on the task index, it copies the content of the internal task
 *          descriptor array element to the given task descriptor.
 *
 * @param   taskIndex   : Index of the task to get the data of.
 * @param   taskData    : Pointer to the task descriptor to save the task data in.
 *
 * @return  Result of task data get.
 *
 * @retval  GOS_SUCCESS : Task data copied successfully.
 * @retval  GOS_ERROR   : Invalid task index or task data pointer is NULL.
 */
gos_result_t gos_taskGetDataByIndex (
        u16_t                 taskIndex,
        gos_taskDescriptor_t* taskData
        );

/**
 * @brief   Returns the number of registered tasks.
 * @details Loops through the internal task array and counts the
 *          entries where a task function is registered.
 *
 * @param   pTaskNum : Variable to store the number of tasks.
 *
 * @return  Success of task number counting.
 *
 * @retval  GOS_SUCCESS : Task number counted successfully.
 * @retval  GOS_ERROR   : Target variable is NULL pointer.
 */
gos_result_t gos_taskGetNumber (
        u16_t* pTaskNum
        );

/**
 * @brief   Yields the current task.
 * @details Invokes rescheduling.
 *
 * @return  Result of task yield.
 *
 * @retval  GOS_SUCCESS : Yield successful.
 */
gos_result_t gos_taskYield (
        void_t
        );

#endif