/*
 * app_debouncer.c
 *
 *  Created on: Dec 8, 2022
 *      Author: Ahmed
 */

/*
 * Includes
 */
#include "app.h"
#include "string.h"

/*
 * Macros
 */
#define DEBOUNCE_TIME_MS ( 300 )

/*
 * Global variables
 */
app_button_block_t buttonBlock;

/*
 * Function prototypes
 */
GOS_STATIC void_t app_debouncerTask (void_t);

/**
 * APP debouncer task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t appDebouncerTaskDesc =
{
	.taskFunction	    = app_debouncerTask,
	.taskName		    = "app_debouncer_task",
	.taskStackSize 	    = 0x200,
	.taskPriority 	    = 25,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: app_debouncerInit
 */
gos_result_t app_debouncerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t debouncerInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	(void_t) memset(buttonBlock.buttonXBlockCounter, 0, sizeof(buttonBlock.buttonXBlockCounter));
	(void_t) memset(buttonBlock.buttonXBlocked, GOS_FALSE, sizeof(buttonBlock.buttonXBlocked));

	debouncerInitResult &= gos_taskRegister(&appDebouncerTaskDesc, NULL);

	if (debouncerInitResult != GOS_SUCCESS)
	{
		debouncerInitResult = GOS_ERROR;
	}

	return debouncerInitResult;
}

/*
 * Function: app_debouncerActivate
 */
gos_result_t app_debouncerActivate (void_t)
{
	/*
	 * Local variables.
	 */
	gos_taskPrivilegeLevel_t callerPrivileges;
	gos_tid_t                callerId;
	gos_result_t             activateResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	activateResult &= gos_taskGetPrivileges(appDebouncerTaskDesc.taskId, &callerPrivileges);

	if ((callerPrivileges & GOS_PRIV_TASK_MANIPULATE) == 0u)
	{
		activateResult &= gos_taskGetCurrentId(&callerId);
		activateResult &= gos_taskAddPrivilege(callerId, GOS_PRIV_TASK_MANIPULATE);
		activateResult &= gos_taskResume(appDebouncerTaskDesc.taskId);
		activateResult &= gos_taskRemovePrivilege(callerId, GOS_PRIV_TASK_MANIPULATE);
	}
	else
	{
		activateResult &= gos_taskResume(appDebouncerTaskDesc.taskId);
	}

	if (activateResult != GOS_SUCCESS)
	{
		activateResult = GOS_ERROR;
	}

	return activateResult;
}

// TODO
GOS_STATIC void_t app_debouncerTask (void_t)
{
	/*
	 * Function code.
	 */
	/*
	 * Suspend task (will be resumed when debouncing is required.
	 */
	(void_t) gos_taskSuspend(appDebouncerTaskDesc.taskId);

	for (;;)
	{
		for (;;)
		{
			/*
			 * Check each button whether they are blocked.
			 * If so, increase their block counter every millisecond.
			 * When the debounce time has elapsed, unblock the button, and
			 * clear the block counter.
			 */
			bool_t debounceRequired = GOS_FALSE;

			for (int i = 0; i < APP_DEBOUNCE_BUTTON_NUM; i++)
			{
				if (buttonBlock.buttonXBlocked[i] == GOS_TRUE)
				{
					debounceRequired = GOS_TRUE;

					if (++buttonBlock.buttonXBlockCounter[i] == DEBOUNCE_TIME_MS)
					{
						buttonBlock.buttonXBlocked[i] = GOS_FALSE;
						buttonBlock.buttonXBlockCounter[i] = 0u;
					}
				}
			}

			if (debounceRequired == GOS_FALSE)
			{
				// If no more debouncing was required, jump out of loop.
				break;
			}
			else
			{
				// Continue debouncing.
				(void_t) gos_taskSleep(1);
			}
		}

		/*
		 * No debouncing is required at this point, suspend task.
		 */
		(void_t) gos_taskSuspend(appDebouncerTaskDesc.taskId);
	}
}
