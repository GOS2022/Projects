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
//! @file       gos_task.c
//! @author     Ahmed Gazar
//! @date       2024-06-13
//! @version    1.2
//!
//! @brief      GOS task source.
//! @details    For a more detailed description of this module, please refer to @ref gos_kernel.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2023-11-06    Ahmed Gazar     Initial version created.
// 1.1        2024-04-19    Ahmed Gazar     *    Task register task CPU limit range check fixed
// 1.2        2024-06-13    Ahmed Gazar     +    gos_taskGetNumber added
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
#include <gos_error.h>
#include <gos_kernel.h>
#include "gos_task.h"
#include <string.h>


/*
 * External variables
 */
GOS_EXTERN u8_t               inIsr;
GOS_EXTERN u32_t              currentTaskIndex;
GOS_EXTERN bool_t             isKernelRunning;
GOS_EXTERN bool_t             forceIdle;

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t  gos_taskCheckDescriptor    (gos_taskDescriptor_t* taskDescriptor);

/*
 * Global function prototypes
 */
void_t                   gos_idleTask               (void_t);

/**
 * Internal task array.
 */
gos_taskDescriptor_t taskDescriptors [CFG_TASK_MAX_NUMBER] =
{
    [0] =
        {
            .taskFunction       = gos_idleTask,
            .taskId             = GOS_DEFAULT_TASK_ID,
            .taskName           = "gos_idle_task",
            .taskPriority       = GOS_TASK_IDLE_PRIO,
            .taskState          = GOS_TASK_READY,
            .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_KERNEL,
        }
};

/*
 * Function: gos_taskRegisterTasks
 */
gos_result_t gos_taskRegisterTasks (gos_taskDescriptor_t* taskDescriptors, u16_t arraySize)
{
    /*
     * Local variables.
     */
    gos_result_t registerResult = GOS_ERROR;
    u16_t        numberOfTasks  = 0u;
    u16_t        index          = 0u;

    /*
     * Function code.
     */
    if (taskDescriptors != NULL)
    {
        numberOfTasks = arraySize / sizeof(taskDescriptors[0]);

        for (index = 0u; index < numberOfTasks; index++)
        {
            if (gos_taskRegister(&taskDescriptors[index], NULL) != GOS_SUCCESS)
            {
                break;
            }
            else
            {
                // Continue.
            }
        }

        if (index == numberOfTasks)
        {
            registerResult = GOS_SUCCESS;
        }
        else
        {
            // There was an error during registering.
        }
    }
    else
    {
        // Task descriptor array is NULL.
    }

    return registerResult;
}

/*
 * Function: gos_taskRegister
 */
gos_result_t gos_taskRegister (gos_taskDescriptor_t* taskDescriptor, gos_tid_t* taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskRegisterResult = GOS_SUCCESS;
    u16_t        taskIndex          = 0u;

    /*
     * Function code.
     */
    // Check descriptor structure.
    if (gos_taskCheckDescriptor(taskDescriptor) == GOS_ERROR)
    {
        taskRegisterResult = GOS_ERROR;
    }
    else
    {
        taskRegisterResult = GOS_ERROR;

        // Find empty slot.
        for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
        {
            if (taskDescriptors[taskIndex].taskFunction == NULL)
            {
                taskRegisterResult = GOS_SUCCESS;
                break;
            }
            else
            {
                // Continue.
            }
        }
        // Check if empty slot was found.
        if (taskIndex >= CFG_TASK_MAX_NUMBER)
        {
            taskRegisterResult = GOS_ERROR;
        }
        else
        {
            // Initial state.
            taskDescriptors[taskIndex].taskState            = GOS_TASK_READY;
            taskDescriptors[taskIndex].taskFunction         = taskDescriptor->taskFunction;
            taskDescriptors[taskIndex].taskPriority         = taskDescriptor->taskPriority;
            taskDescriptors[taskIndex].taskOriginalPriority = taskDescriptor->taskPriority;
            taskDescriptors[taskIndex].taskId               = (GOS_DEFAULT_TASK_ID + taskIndex);
            taskDescriptors[taskIndex].taskPrivilegeLevel   = taskDescriptor->taskPrivilegeLevel;

            // Copy task name.
            if (taskDescriptor->taskName != NULL &&
                strlen(taskDescriptor->taskName) <= CFG_TASK_MAX_NAME_LENGTH)
            {
                (void_t) strcpy(taskDescriptors[taskIndex].taskName, taskDescriptor->taskName);
            }
            else
            {
                // Task name is not requried.
            }

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
        }
    }

    return taskRegisterResult;
}

/*
 * Function: gos_taskSleep
 */
GOS_INLINE gos_result_t gos_taskSleep (gos_taskSleepTick_t sleepTicks)
{
    /*
     * Local variables.
     */
    gos_result_t taskSleepResult = GOS_ERROR;

    /*
     * Function code.
     */
    // TODO: support pre-os delays (?)
    if (isKernelRunning == GOS_FALSE)
    {
    	gos_kernelDelayMs(sleepTicks);
    }
    else
    {
        if (currentTaskIndex > 0u)
        {
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
    }

    return taskSleepResult;
}

/*
 * Function: gos_taskWakeup
 */
GOS_INLINE gos_result_t gos_taskWakeup (gos_tid_t taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskWakeupResult = GOS_ERROR;
    u32_t        taskIndex        = 0u;

    /*
     * Function code.
     */    
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check task manipulation privilege.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            inIsr > 0)
        {
            if (taskDescriptors[taskIndex].taskState == GOS_TASK_SLEEPING)
            {
                taskDescriptors[taskIndex].taskState = GOS_TASK_READY;
                taskWakeupResult = GOS_SUCCESS;
            }
            else
            {
                // Nothing to do.
            }
        }
        else
        {
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to wake up <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
        }
    }
    else
    {
        // Task ID error.
    }    

    return taskWakeupResult;
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
            currentTaskIndex == taskIndex || inIsr > 0)
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
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to suspend <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
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
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            inIsr > 0)
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
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to resume <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
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
            currentTaskIndex == taskIndex || inIsr > 0)
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
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to block <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
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

        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            inIsr > 0)
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
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to unblock <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
        }
    }
    else
    {
        // Task ID error.
    }

    return taskUnblockResult;
}

/*
 * Function: gos_taskDelete
 */
GOS_INLINE gos_result_t gos_taskDelete (gos_tid_t taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskDeleteResult = GOS_ERROR;
    u32_t        taskIndex         = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check task manipulation privilege.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_MANIPULATE) == GOS_PRIV_TASK_MANIPULATE ||
            currentTaskIndex == taskIndex || inIsr > 0)
        {
            if (taskDescriptors[taskIndex].taskState != GOS_TASK_ZOMBIE)
            {
                taskDescriptors[taskIndex].taskState = GOS_TASK_ZOMBIE;
                taskDeleteResult = GOS_SUCCESS;

                // Invoke signal.
                if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_SIGNALING) != GOS_PRIV_SIGNALING)
                {
                    taskDescriptors[currentTaskIndex].taskPrivilegeLevel |= GOS_PRIV_SIGNALING;
                    //(void_t) gos_signalInvoke(kernelTaskDeleteSignal, taskId);
                    taskDescriptors[currentTaskIndex].taskPrivilegeLevel &= ~GOS_PRIV_SIGNALING;
                }
                else
                {
                    //(void_t) gos_signalInvoke(kernelTaskDeleteSignal, taskId);
                }
            }
            else
            {
                // Nothing to do.
            }
        }
        else
        {
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to delete <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
        }
    }
    else
    {
        // Task ID error.
    }    

    if (taskDeleteResult == GOS_SUCCESS)
    {
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

    return taskDeleteResult;
}

/*
 * Function: gos_taskSetPriority
 */
GOS_INLINE gos_result_t gos_taskSetPriority (gos_tid_t taskId, gos_taskPrio_t taskPriority)
{
    /*
     * Local variables.
     */
    gos_result_t taskSetPriorityResult = GOS_ERROR;
    u32_t        taskIndex             = 0u;

    /*
     * Function code.
     */    
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskPriority < GOS_TASK_MAX_PRIO_LEVELS)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check privilege level.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_PRIO_CHANGE) == GOS_PRIV_TASK_PRIO_CHANGE ||
            inIsr > 0)
        {
            taskDescriptors[taskIndex].taskPriority = taskPriority;
            taskSetPriorityResult = GOS_SUCCESS;
        }
        else
        {
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to set the priority of <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
        }
    }
    else
    {
        // Task ID or task priority error.
    }    

    return taskSetPriorityResult;
}

/*
 * Function: gos_taskSetOriginalPriority
 */
GOS_INLINE gos_result_t gos_taskSetOriginalPriority (gos_tid_t taskId, gos_taskPrio_t taskPriority)
{
    /*
     * Local variables.
     */
    gos_result_t taskSetPriorityResult = GOS_ERROR;
    u32_t        taskIndex             = 0u;

    /*
     * Function code.
     */    
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskPriority < GOS_TASK_MAX_PRIO_LEVELS)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        // Check privilege level.
        if ((taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_PRIV_TASK_PRIO_CHANGE) == GOS_PRIV_TASK_PRIO_CHANGE ||
            inIsr > 0)
        {
            taskDescriptors[taskIndex].taskOriginalPriority = taskPriority;
            taskSetPriorityResult = GOS_SUCCESS;
        }
        else
        {            
            gos_errorHandler(GOS_ERROR_LEVEL_OS_FATAL, __func__, __LINE__, "<%s> has no privilege to set the priority of <%s>!",
                taskDescriptors[currentTaskIndex].taskName,
                taskDescriptors[taskIndex].taskName
            );
        }
    }
    else
    {
        // Task ID or task priority error.
    }    

    return taskSetPriorityResult;
}

/*
 * Function: gos_taskGetPriority
 */
gos_result_t gos_taskGetPriority (gos_tid_t taskId, gos_taskPrio_t* taskPriority)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetPriorityResult = GOS_ERROR;
    u32_t        taskIndex             = 0u;

    /*
     * Function code.
     */    
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskPriority != NULL)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);
        *taskPriority = taskDescriptors[taskIndex].taskPriority;
        taskGetPriorityResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error or task priority is NULL.
    }    

    return taskGetPriorityResult;
}

/*
 * Function: gos_taskGetOriginalPriority
 */
gos_result_t gos_taskGetOriginalPriority (gos_tid_t taskId, gos_taskPrio_t* taskPriority)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetPriorityResult = GOS_ERROR;
    u32_t        taskIndex             = 0u;

    /*
     * Function code.
     */    
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskPriority != NULL)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);
        *taskPriority = taskDescriptors[taskIndex].taskOriginalPriority;
        taskGetPriorityResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error or task priority is NULL.
    }    

    return taskGetPriorityResult;
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
 * Function: gos_taskYield
 */
GOS_INLINE gos_result_t gos_taskYield (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t taskYieldResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    cont_yield(&taskDescriptors[currentTaskIndex].taskContext);
    return taskYieldResult;
}

/*
 * Function: gos_taskGetName
 */
gos_result_t gos_taskGetName (gos_tid_t taskId, gos_taskName_t taskName)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetNameResult = GOS_ERROR;
    u32_t        taskIndex         = 0u;

    /*
     * Function code.
     */
    if (taskId > GOS_DEFAULT_TASK_ID && (taskId - GOS_DEFAULT_TASK_ID) < CFG_TASK_MAX_NUMBER &&
        taskName != NULL)
    {
        taskIndex = (u32_t)(taskId - GOS_DEFAULT_TASK_ID);

        (void_t) strcpy(taskName, taskDescriptors[taskIndex].taskName);

        taskGetNameResult = GOS_SUCCESS;
    }
    else
    {
        // Task ID error or task name pointer is NULL.
    }

    return taskGetNameResult;
}

/*
 * Function: gos_taskGetId
 */
gos_result_t gos_taskGetId (gos_taskName_t taskName, gos_tid_t* taskId)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetIdResult = GOS_ERROR;
    u32_t        taskIndex       = 0u;

    /*
     * Function code.
     */
    for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER && taskId != NULL; taskIndex++)
    {
        if (strcmp(taskName, taskDescriptors[taskIndex].taskName) == 0u)
        {
            *taskId = taskDescriptors[taskIndex].taskId;
            taskGetIdResult = GOS_SUCCESS;
            break;
        }
        else
        {
            // Continue.
        }
    }

    return taskGetIdResult;
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

        (void_t) memcpy((void*)taskData, (void*)&taskDescriptors[taskIndex], sizeof(*taskData));

        taskGetDataResult = GOS_SUCCESS;
    }
    else
    {
        // Task data does not exist.
    }

    return taskGetDataResult;
}

/*
 * Function: gos_taskGetDataByIndex
 */
gos_result_t gos_taskGetDataByIndex (u16_t taskIndex, gos_taskDescriptor_t* taskData)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetDataResult = GOS_ERROR;

    /*
     * Function code.
     */    
    if (taskIndex < CFG_TASK_MAX_NUMBER &&
        taskData != NULL &&
        (taskDescriptors[currentTaskIndex].taskPrivilegeLevel & GOS_TASK_PRIVILEGE_KERNEL) == GOS_TASK_PRIVILEGE_KERNEL)
    {
        (void_t) memcpy((void*)taskData, (void*)&taskDescriptors[taskIndex], sizeof(*taskData));

        taskGetDataResult = GOS_SUCCESS;
    }
    else
    {
        // Task data does not exist.
    }    

    return taskGetDataResult;
}

/*
 * Function: gos_taskGetNumber
 */
gos_result_t gos_taskGetNumber (u16_t* pTaskNum)
{
    /*
     * Local variables.
     */
    gos_result_t taskGetNumResult = GOS_ERROR;
    u16_t        taskIndex        = 0u;

    /*
     * Function code.
     */
    if (pTaskNum != NULL)
    {
        // Initialize variable.
        *pTaskNum = 0u;

        for (taskIndex = 0u; taskIndex < CFG_TASK_MAX_NUMBER; taskIndex++)
        {
            if (taskDescriptors[taskIndex].taskFunction != NULL)
            {
                // Increase counter.
                (*pTaskNum)++;
            }
            else
            {
                // Last task found.
                break;
            }
        }

        taskGetNumResult = GOS_SUCCESS;
    }
    else
    {
        // NULL pointer.
    }

    return taskGetNumResult;
}

/**
 * @brief   Checks the validity of the task descriptor.
 * @details It checks the task function pointer, task priority value, stack, and
 *          size parameters and return with error if the parameters are incorrect.
 *
 * @param   taskDescriptor : Pointer to the task descriptor structure.
 *
 * @return  Result of task descriptor checking.
 *
 * @retval  GOS_SUCCESS    :    Descriptor contains valid data.
 * @retval  GOS_ERROR      :    One or more parameter is invalid:
 *                              - Task function is NULL pointer
 *                              - Task function is the idle task
 *                              - Priority exceeds the maximum priority level
 *                              - Stack size is smaller than the minimum allowed
 *                              - Stack size is greater than the maximum allowed
 *                              - Stack size is not 4-byte aligned
 */
GOS_STATIC gos_result_t gos_taskCheckDescriptor (gos_taskDescriptor_t* taskDescriptor)
{
    /*
     * Local variables.
     */
    gos_result_t taskDescCheckResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    if (taskDescriptor->taskFunction == NULL                    ||
        taskDescriptor->taskPrivilegeLevel == 0                 ||
        taskDescriptor->taskPriority > GOS_TASK_MAX_PRIO_LEVELS ||
        taskDescriptor->taskFunction == gos_idleTask)
    {
        taskDescCheckResult = GOS_ERROR;
    }
    else
    {
        // Task descriptor OK.
    }

    return taskDescCheckResult;
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
    u32_t prevMillis = millis();

    for (;;)
    {
        (void_t) gos_taskYield();

        u16_t timePassed = (u16_t)(millis() - prevMillis);
        prevMillis = millis();
#include "gos_time.h"
        (void_t) gos_timeIncreaseSystemTime(timePassed);
    }
}
