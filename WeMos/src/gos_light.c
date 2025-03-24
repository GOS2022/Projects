#if 0
#include "gos_light.h"
#include <core_esp8266_features.h>
#include <string.h>
#include "gos_time.h"
#include <gos_error.h>
#include <gos_trace.h>

#define CFG_TASK_MAX_NUMBER ( 4u )

/**
 * Scheduling disabled counter.
 */
u8_t                                schedDisableCntr             = 0u;
/**
 * Current task index - for priority scheduling.
 */
u32_t                               currentTaskIndex             = 0u;

/**
 * TODO
 */
GOS_STATIC u32_t previousTick;

GOS_STATIC bool_t forceIdle;

/*
 * Global variables
 */

GOS_STATIC void_t gos_kernelSelectNextTask (void_t);

/*
 * Global function prototypes
 */
void_t                   gos_idleTask               (void_t);

/**
 * Internal task array.
 */
gos_taskDescriptor_t taskDescriptors [CFG_TASK_MAX_NUMBER];

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

void_t gos_kernelStart (void_t)
{
    if (forceIdle == GOS_TRUE)
    {
        currentTaskIndex = 0;
    }
    else
    {
        gos_kernelSelectNextTask();
    }

    cont_run(&taskDescriptors[currentTaskIndex].taskContext, taskDescriptors[currentTaskIndex].taskFunction);
}

/*
 * Function: gos_taskGetCurrentId
 */
GOS_INLINE gos_result_t gos_taskGetCurrentId (gos_tid_t* taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetCurrentIdResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (taskId != NULL)
    {
        *taskId = (gos_tid_t)(GOS_DEFAULT_TASK_ID + currentTaskIndex);
        taskGetCurrentIdResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID pointer is NULL.
    }

    return taskGetCurrentIdResult;
}

/*
 * Function: gos_taskGetData
 */
gos_result_t gos_taskGetData (gos_tid_t taskId, gos_taskDescriptor_t* taskData)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetDataResult = GOS_ERROR;
    u32_t        taskIndex         = 0u;

    /*
     * Function code.
     */
    if (taskId >= GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskData != NULL)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        (void_t) memcpy((void_t*)taskData, (void_t*)&taskDescriptors[taskIndex], sizeof(*taskData));
        //strcpy(taskData->taskName, taskDescriptors[taskIndex].taskName);
        //taskData->taskFunction = taskDescriptors[taskIndex].taskFunction;
        //taskData->taskPriority = taskDescriptors[taskIndex].taskPriority;
        //taskData->taskPrivilegeLevel = taskDescriptors[taskIndex].taskPrivilegeLevel;
        //taskData->taskId = taskDescriptors[taskIndex].taskId;

        taskGetDataResult = GOS_SUCCESS;
    }
    else
    {
        // Task data does not exist.
    }

    return taskGetDataResult;
}

/*
 * Function: gos_taskGetPrivileges
 */
GOS_INLINE gos_result_t gos_taskGetPrivileges (gos_tid_t taskId, gos_taskPrivilegeLevel_t* privileges)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetPrivilegesResult = GOS_ERROR;
    u32_t        taskIndex               = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        privileges != NULL)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        *privileges = taskDescriptors[taskIndex].taskPrivilegeLevel;
        taskGetPrivilegesResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error or privileges is NULL pointer.
    }

    return taskGetPrivilegesResult;
}

/*
 * Function: gos_taskSetPrivileges
 */
GOS_INLINE gos_result_t gos_taskSetPrivileges (gos_tid_t taskId, gos_taskPrivilegeLevel_t privileges)
{
    /*
     * Local variables.
     */
    gos_result_t taskSetPrivilegesResult = GOS_ERROR;
    u32_t        taskIndex               = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);
        taskDescriptors[taskIndex].taskPrivilegeLevel = privileges;
        taskSetPrivilegesResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error.
    }

    return taskSetPrivilegesResult;
}

/*
 * Function: gos_taskAddPrivilege
 */
GOS_INLINE gos_result_t gos_taskAddPrivilege (gos_tid_t taskId, gos_taskPrivilegeLevel_t privileges)
{
    /*
     * Local variables.
     */
    gos_result_t taskAddPrivilegeResult = GOS_ERROR;
    u32_t        taskIndex              = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);
        taskDescriptors[taskIndex].taskPrivilegeLevel |= privileges;
        taskAddPrivilegeResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error.
    }

    return taskAddPrivilegeResult;
}

/*
 * Function: gos_taskRemovePrivilege
 */
GOS_INLINE gos_result_t gos_taskRemovePrivilege (gos_tid_t taskId, gos_taskPrivilegeLevel_t privileges)
{
    /*
     * Local variables.
     */
    gos_result_t taskRemovePrivilegeResult = GOS_ERROR;
    u32_t        taskIndex                 = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);
        taskDescriptors[taskIndex].taskPrivilegeLevel &= ~privileges;
        taskRemovePrivilegeResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error.
    }

    return taskRemovePrivilegeResult;
}

gos_result_t gos_taskRegister (gos_taskDescriptor_t* taskDescriptor, gos_tid_t* taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskRegisterResult = GOS_ERROR;
    u16_t        taskIndex          = 0u;

        // Find empty slot.
        for (taskIndex = 1u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
        {
            if (taskDescriptors[taskIndex].taskFunction == NULL)
            {
                // Initial state.
                taskDescriptors[taskIndex].taskState            = GOS_TASK_READY;
                taskDescriptors[taskIndex].taskFunction         = taskDescriptor->taskFunction;
                taskDescriptors[taskIndex].taskPriority         = taskDescriptor->taskPriority;
                taskDescriptors[taskIndex].taskOriginalPriority = taskDescriptor->taskPriority;
                taskDescriptors[taskIndex].taskId               = (GOS_DEFAULT_TASK_ID + taskIndex);
                taskDescriptors[taskIndex].taskPrivilegeLevel   = taskDescriptor->taskPrivilegeLevel;

                // Copy task name.
                /*if (taskDescriptor->taskName != NULL &&
                    strlen(taskDescriptor->taskName) <= CFG_TASK_MAX_NAME_LENGTH)
                {
                    (void_t) strcpy(taskDescriptors[taskIndex].taskName, taskDescriptor->taskName);
                }
                else
                {  
                    // Task name is not requried.
                }*/

                // Set task ID.
                taskDescriptor->taskId = taskDescriptors[taskIndex].taskId;
                if (taskId != NULL)
                {
                    *taskId = taskDescriptors[taskIndex].taskId;
                }
                else
                {
                    // External task ID not required.
                }       

                cont_init(&taskDescriptors[taskIndex].taskContext);

                taskRegisterResult = GOS_SUCCESS;         
                break;
            }
            else
            {
                // Continue.
            }
        }

    return taskRegisterResult;
}

gos_result_t gos_taskSleep (gos_taskSleepTick_t sleepTicks)
{
    /*
     * Local variables.
     */
    gos_result_t taskSleepResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (currentTaskIndex > 0u)
    {
        if (taskDescriptors[currentTaskIndex].taskState == GOS_TASK_READY)
        {
            taskDescriptors[currentTaskIndex].taskState            = GOS_TASK_SLEEPING;
            taskDescriptors[currentTaskIndex].taskSleepTicks       = sleepTicks;
            taskDescriptors[currentTaskIndex].taskSleepTickCounter = 0u;
            taskSleepResult = GOS_SUCCESS;
        }
        else
        {
            // Nothing to do.
        }

        if (taskSleepResult == GOS_SUCCESS)
        {
            cont_yield(&taskDescriptors[currentTaskIndex].taskContext);
        }
        else
        {
            // Nothing to do.
        }
    }
    else
    {
        // Idle task cannot sleep.
    }

    return taskSleepResult;
}

/*
 * Function: gos_taskSuspend
 */
GOS_INLINE gos_result_t gos_taskSuspend (gos_tid_t taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskSuspendResult = GOS_ERROR;
    u32_t        taskIndex         = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check task manipulation privilege.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            currentTaskIndex == taskIndex /*|| inIsr > 0*/)
        {
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_READY ||
                taskDescriptors[taskIndex].taskState == GOS_TASK_SLEEPING ||
                taskDescriptors[taskIndex].taskState == GOS_TASK_BLOCKED)
            {
                taskDescriptors[taskIndex].taskPreviousState = taskDescriptors[taskIndex].taskState;
                taskDescriptors[taskIndex].taskState = GOS_TASK_SUSPENDED;
                taskSuspendResult = GOS_SUCCESS;

                if (currentTaskIndex == taskIndex)
                {
                    cont_yield(&taskDescriptors[currentTaskIndex].taskContext);
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
        else
        {
            /*gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to suspend <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );*/
        }
    }
    else
    {
        // Task ID error.
    }

    return taskSuspendResult;
}

/*
 * Function: gos_taskResume
 */
GOS_INLINE gos_result_t gos_taskResume (gos_tid_t taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskResumeResult = GOS_ERROR;
    u32_t        taskIndex        = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check task manipulation privilege.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE /*||
            inIsr > 0*/)
        {
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_SUSPENDED)
            {
                taskDescriptors[taskIndex].taskState = taskDescriptors[taskIndex].taskPreviousState;
                taskResumeResult = GOS_SUCCESS;
            }
            else
            {
                // Nothing to do.
            }
        }
        else
        {
            /*gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to resume <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );*/
        }
    }
    else
    {
        // Task ID error.
    }

    return taskResumeResult;
}

/*
 * Function: gos_taskBlock
 */
GOS_INLINE gos_result_t gos_taskBlock (gos_tid_t taskId, gos_blockMaxTick_t blockTicks)
{
    /*
     * Local variables.
     */
    gos_result_t taskBlockResult = GOS_ERROR;
    u32_t        taskIndex       = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            currentTaskIndex == taskIndex /*|| inIsr > 0*/)
        {
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_READY)
            {
                taskDescriptors[taskIndex].taskState            = GOS_TASK_BLOCKED;
                taskDescriptors[taskIndex].taskBlockTicks       = blockTicks;
                taskDescriptors[taskIndex].taskBlockTickCounter = 0u;

                taskBlockResult = GOS_SUCCESS;

                if (currentTaskIndex == taskIndex)
                {
                    cont_yield(&taskDescriptors[currentTaskIndex].taskContext);
                }
            }
            else
            {
                // Nothing to do.
            }
        }
        else
        {
            /*gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to block <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );*/
        }
    }
    else
    {
        // Task ID error.
    }

    return taskBlockResult;
}

/*
 * Function: gos_taskUnblock
 */
GOS_INLINE gos_result_t gos_taskUnblock (gos_tid_t taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskUnblockResult = GOS_ERROR;
    u32_t        taskIndex         = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE /*||
            inIsr > 0*/)
        {
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_BLOCKED)
            {
                taskDescriptors[taskIndex].taskState = GOS_TASK_READY;
                taskUnblockResult = GOS_SUCCESS;
            }
            else if (taskDescriptors[taskIndex].taskState == GOS_TASK_SUSPENDED &&
                    taskDescriptors[taskIndex].taskPreviousState == GOS_TASK_BLOCKED)
            {
                taskDescriptors[taskIndex].taskPreviousState = GOS_TASK_READY;
                taskUnblockResult = GOS_SUCCESS;
            }
            else
            {
                // Nothing to do.
            }
        }
        else
        {
            /*gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to unblock <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );*/
        }
    }
    else
    {
        // Task ID error.
    }
    
    return taskUnblockResult;
}

/**
 * @brief   Kernel idle task.
 * @details This task is executed when there is no other ready task in the system.
 *          When executed, this function refreshes the CPU-usage statistics of tasks.
 *
 * @return    -
 */
void_t gos_idleTask (void_t)
{
    /*
     * Function code.
     */
    (void_t) gos_errorTraceInit("Starting OS...", GOS_SUCCESS);
    forceIdle = GOS_FALSE;

    for (;;)
    {
        cont_yield(&taskDescriptors[0].taskContext);
    }
}

GOS_STATIC void_t gos_kernelSelectNextTask (void_t)
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

u32_t gos_kernelGetSysTicks (void_t)
{
    return millis();
}
#endif