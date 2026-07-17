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
//! @file       gos_shell.c
//! @author     Ahmed Gazar
//! @date       2026-07-17
//! @version    1.12
//!
//! @brief      GOS shell service source.
//! @details    For a more detailed description of this service, please refer to @ref gos_shell.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2022-11-04    Ahmed Gazar     Initial version created
// 1.1        2022-11-15    Ahmed Gazar     +    License added
// 1.2        2022-12-03    Ahmed Gazar     +    gos_shellRegisterCommands added
//                                          *    Actual command and command parameter buffers
//                                               made modul global instead of task local
// 1.3        2022-12-11    Ahmed Gazar     +    Built-in shell commands (dump and reset) added
// 1.4        2022-12-13    Ahmed Gazar     +    Privilege handling added
// 1.5        2023-01-12    Ahmed Gazar     +    Task delete, suspend, resume, help commands added
// 1.6        2023-05-19    Ahmed Gazar     *    Command buffer size modified
//                                          +    Buffer overflow protection added
// 1.7        2023-07-12    Ahmed Gazar     +    Command handler privilege-handling added
// 1.8        2023-09-08    Ahmed Gazar     +    Shell help: list of shell commands added
//                                          +    Shell CPU and runtime commands added
// 1.9        2024-06-28    Ahmed Gazar     +    Task unblock commands added
// 1.10       2025-07-29    Ahmed Gazar     +    CFG_SHELL_STARTUP_DELAY_MS added
// 1.11       2026-07-16    Ahmed Gazar     *    Backspace handling fix, major rework
//                                          +    VT100 features introduced
// 1.12       2026-07-17    Ahmed Gazar		*    Major fixes and user handling introduced
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
#include <gos.h>
#include <gos_shell.h>
#include <gos_shell_driver.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Macros
 */
/**
 * Shell daemon poll time [ms].
 */
#define GOS_SHELL_DAEMON_POLL_TIME_MS    ( 50u )

/**
 * Shell display text.
 */
#define GOS_SHELL_DISPLAY_TEXT           ("[\x1B[1m\x1B[33mgos shell\x1B[0m]>> ")

/**
 * TODO
 */
#define GOS_SHELL_ESCAPE_CHAR            ((char)0x1Bu)

/**
 * TODO
 */
#define GOS_SHELL_MAX_USERS              (8u)

/**
 * TODO
 */
#define GOS_SHELL_USERNAME_MAX_LEN       (32u)

/**
 * TODO
 */
#define GOS_SHELL_PASSWORD_MAX_LEN       (32u)

/**
 * History buffer size configuration.
 */
#define CFG_SHELL_HISTORY_SIZE           (8u)

/*
 * Type definitions
 */
/**
 * Input states for processing special characters.
 */
typedef enum
{
    SHELL_INPUT_NORMAL,
    SHELL_INPUT_ESC,
    SHELL_INPUT_CSI
} shellInputState_t;

/* Add after SHELL_INPUT_CSI definition */
typedef enum
{
    SHELL_CMD_INPUT_NORMAL,
    SHELL_CMD_INPUT_PASSWORD
} shellCmdInputState_t;

/*
 * User structure.
 */
typedef struct
{
    char_t username [GOS_SHELL_USERNAME_MAX_LEN];
    char_t password [GOS_SHELL_PASSWORD_MAX_LEN];
    gos_shellUserPrivilege_t privilege;
    bool_t isEnabled;
} gos_shellUser_t;

/*
 * Static variables
 */
/**
 * Shell command array.
 */
GOS_STATIC gos_shellCommand_t shellCommands        [CFG_SHELL_MAX_COMMAND_NUMBER];

/**
 * Shell daemon task ID.
 */
GOS_STATIC gos_tid_t          shellDaemonTaskId;

/**
 * Command buffer.
 */
GOS_STATIC char_t             commandBuffer        [CFG_SHELL_COMMAND_BUFFER_SIZE];

/**
 * Command buffer index.
 */
GOS_STATIC u16_t              commandBufferIndex;

/**
 * Command buffer length.
 */
GOS_STATIC u16_t              commandBufferLength;

/**
 * Shell echo flag.
 */
GOS_STATIC bool_t             useEcho;

/**
 * Actual command buffer.
 */
GOS_STATIC char_t             actualCommand        [CFG_SHELL_MAX_COMMAND_LENGTH];

/**
 * TODO
 */
GOS_STATIC char_t             previousCommand      [CFG_SHELL_MAX_COMMAND_LENGTH];

/**
 * Command parameters buffer.
 */
GOS_STATIC char_t             commandParams        [CFG_SHELL_MAX_PARAMS_LENGTH];

/**
 * TODO
 */
GOS_STATIC shellInputState_t  inputState = SHELL_INPUT_NORMAL;

/* Add after inputState declaration */
GOS_STATIC shellCmdInputState_t cmdInputState = SHELL_CMD_INPUT_NORMAL;

/**
 * TODO
 */
GOS_STATIC char_t             historyBuffer [CFG_SHELL_HISTORY_SIZE][CFG_SHELL_COMMAND_BUFFER_SIZE];

/**
 * TODO
 */
GOS_STATIC u16_t              historyCount;

/**
 * TODO
 */
GOS_STATIC u16_t              historyCursor;

/**
 * TODO
 */
GOS_STATIC gos_shellUser_t    registeredUsers [GOS_SHELL_MAX_USERS];

/**
 * TODO
 */
GOS_STATIC u16_t              registeredUsersCount = 0u;

/**
 * TODO
 */
GOS_STATIC gos_shellUser_t    currentUser;

/**
 * TODO
 */
GOS_STATIC bool_t             isAuthenticated = GOS_FALSE;

GOS_STATIC bool_t             isPasswordInputMode = GOS_FALSE;

/* Root user (always available) */
GOS_STATIC gos_shellUser_t    rootUser =
{
    .username = "root",
    .password = "root",
    .privilege = GOS_SHELL_USER_PRIVILEGE_ROOT,
    .isEnabled = GOS_TRUE
};

/*
 * Function prototypes
 */
GOS_STATIC void_t gos_shellDaemonTask     (void_t);
GOS_STATIC void_t gos_shellCommandHandler (char_t* params);
GOS_STATIC void_t gos_shellRefreshCommandLine (void_t);
GOS_STATIC bool_t gos_shellCanExecuteCommand(gos_shellUserPrivilege_t requiredPrivilege);
GOS_STATIC void_t gos_shellPasswordInputEnd (void_t);
GOS_STATIC void_t gos_shellPasswordInputStart (bool_t hideInput);
GOS_STATIC bool_t gos_shellAuthenticateUser (GOS_CONST char_t* username, GOS_CONST char_t* password);
GOS_STATIC void_t gos_shellLoginCommand (char_t* params);
GOS_STATIC void_t gos_shellLogoutCommand (char_t* params);

GOS_STATIC void_t gos_shellRefreshCommandLine (void_t)
{
    if (useEcho == GOS_TRUE)
    {
        (void_t) gos_shellDriverTransmitString("\r%s%s\x1B[K",
                                               GOS_SHELL_DISPLAY_TEXT,
                                               commandBuffer);

        if (commandBufferIndex < commandBufferLength)
        {
            (void_t) gos_shellDriverTransmitString("\x1B[%uD",
                                                   (u32_t)(commandBufferLength - commandBufferIndex));
        }
    }
}

static GOS_INLINE void gos_shellHistoryInit (void)
{
    historyCount  = 0u;
    historyCursor = 0u;
}

static GOS_INLINE void gos_shellHistoryPush (const char_t* command)
{
    if (command == NULL || command[0] == '\0')
    {
        return;
    }

    if (historyCount > 0u &&
        strcmp(historyBuffer[historyCount - 1u], command) == 0)
    {
        historyCursor = 0u;
        return;
    }

    if (historyCount < CFG_SHELL_HISTORY_SIZE)
    {
        (void_t) strcpy(historyBuffer[historyCount++], command);
    }
    else
    {
        (void_t) memmove(historyBuffer,
                        historyBuffer + 1u,
                        sizeof(historyBuffer[0]) * (CFG_SHELL_HISTORY_SIZE - 1u));
        (void_t) strcpy(historyBuffer[CFG_SHELL_HISTORY_SIZE - 1u], command);
    }

    historyCursor = 0u;
}

static GOS_INLINE const char_t* gos_shellHistoryGet (u16_t cursor)
{
    if (cursor == 0u || cursor > historyCount)
    {
        return NULL;
    }

    return historyBuffer[historyCount - cursor];
}

static GOS_INLINE void gos_shellHistoryResetBrowse (void)
{
    historyCursor = 0u;
}

/**
 * Shell daemon task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t shellDaemonTaskDesc =
{
    .taskFunction        = gos_shellDaemonTask,
    .taskName            = "gos_shell_daemon",
    .taskPriority        = CFG_TASK_SHELL_DAEMON_PRIO,
    .taskStackSize       = CFG_TASK_SHELL_DAEMON_STACK,
    .taskPrivilegeLevel  = GOS_TASK_PRIVILEGE_KERNEL
};

/**
 * Shell info command (built-in, requires ROOT)
 */
GOS_STATIC gos_shellCommand_t shellCommand =
{
    .command        = "shell",
    .commandHandler = gos_shellCommandHandler,
	.commandHandlerPrivileges = GOS_TASK_PRIVILEGE_KERNEL,
    .commandPrivilege = GOS_SHELL_USER_PRIVILEGE_ROOT
};

/*
 * Function: gos_shellInit
 */
gos_result_t gos_shellInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t            shellInitResult = GOS_ERROR;
    gos_shellCommandIndex_t index           = 0u;

    /*
     * Function code.
     */
    useEcho = GOS_TRUE;

    /* Initialize entire shellCommands array and all buffers to safe defaults */
    (void_t) memset(shellCommands, 0, sizeof(shellCommands));
    (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
    (void_t) memset(actualCommand, 0, sizeof(actualCommand));
    (void_t) memset(previousCommand, 0, sizeof(previousCommand));
    (void_t) memset(commandParams, 0, sizeof(commandParams));
    commandBufferIndex = 0u;
    commandBufferLength = 0u;
    inputState = SHELL_INPUT_NORMAL;
    gos_shellHistoryInit();
    (void_t) memset(&currentUser, 0, sizeof(gos_shellUser_t));

    for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
    {
        /* already zeroed above, but keep setting handler explicitly for clarity */
        shellCommands[index].commandHandler = NULL;
    }

    if (gos_taskRegister(&shellDaemonTaskDesc, &shellDaemonTaskId) == GOS_SUCCESS &&
        gos_shellRegisterCommand(&shellCommand) == GOS_SUCCESS)
    {
        shellInitResult = GOS_SUCCESS;
    }
    else
    {
        // Nothing to do.
    }

    return shellInitResult;
}

/*
 * Function: gos_shellRegisterCommands
 */
gos_result_t gos_shellRegisterCommands (gos_shellCommand_t* commands, u16_t arraySize)
{
    /*
     * Local variables.
     */
    gos_result_t registerResult   = GOS_ERROR;
    u16_t        numberOfCommands = 0u;
    u16_t        index            = 0u;

    /*
     * Function code.
     */
    if (commands != NULL)
    {
        numberOfCommands = arraySize / sizeof(commands[0]);

        for (index = 0u; index < numberOfCommands; index++)
        {
            if (gos_shellRegisterCommand(&commands[index]) != GOS_SUCCESS)
            {
                break;
            }
            else
            {
                // Nothing to do.
            }
        }

        if (index == numberOfCommands)
        {
            registerResult = GOS_SUCCESS;
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

    return registerResult;
}

/*
 * Function: gos_shellRegisterCommand
 */
gos_result_t gos_shellRegisterCommand (gos_shellCommand_t* command)
{
    /*
     * Local variables.
     */
    gos_result_t            shellRegisterCommandResult = GOS_ERROR;
    gos_shellCommandIndex_t index                      = 0u;

    /*
     * Function code.
     */
    if (command->commandHandler != NULL && command->command != NULL)
    {
        for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
        {
            if (shellCommands[index].commandHandler == NULL)
            {
                shellCommands[index].commandHandler = command->commandHandler;
                shellCommands[index].commandHandlerPrivileges = command->commandHandlerPrivileges;
                shellCommands[index].commandPrivilege = command->commandPrivilege;
                (void_t) strcpy(shellCommands[index].command, command->command);
                shellRegisterCommandResult = GOS_SUCCESS;
                break;
            }
            else
            {
                // Nothing to do.
            }
        }
    }
    return shellRegisterCommandResult;
}

/*
 * Function: gos_shellSuspend
 */
gos_result_t gos_shellSuspend (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t shellSuspendResult = GOS_ERROR;

    /*
     * Function code.
     */
    shellSuspendResult = gos_taskSuspend(shellDaemonTaskId);

    return shellSuspendResult;
}

/*
 * Function: gos_shellResume
 */
gos_result_t gos_shellResume (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t shellResumeResult = GOS_ERROR;

    /*
     * Function code.
     */
    shellResumeResult = gos_taskResume(shellDaemonTaskId);

    return shellResumeResult;
}

/*
 * Function: gos_shellEchoOn
 */
gos_result_t gos_shellEchoOn (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t shellEchoOnResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    useEcho = GOS_TRUE;

    return shellEchoOnResult;
}

/*
 * Function: gos_shellEchoOff
 */
gos_result_t gos_shellEchoOff (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t shellEchoOffResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    useEcho = GOS_FALSE;

    return shellEchoOffResult;
}

/**
 * @brief   Shell daemon task.
 * @details Receives a character from the log serial line, if the echoing is on, then
 *          sends the same character back. When an enter key is received, it processes
 *          the command typed in, and loops through the command array. When the command
 *          is found, it calls the command handler function with the parameter list as
 *          a string.
 *
 * @return    -
 */
GOS_STATIC void_t gos_shellDaemonTask (void_t)
{
    /*
     * Local variables.
     */
    gos_shellCommandIndex_t index              = 0u;
    u16_t                   paramIndex         = 0u;
    gos_time_t              systime;

    /*
     * Function code.
     */
    (void_t) gos_taskSleep(CFG_SHELL_STARTUP_DELAY_MS);
    (void_t) gos_shellDriverTransmitString(GOS_SHELL_DISPLAY_TEXT);

    for (;;)
    {
        char_t rx = '\0';

        if (gos_shellDriverReceiveChar(&rx) == GOS_SUCCESS)
        {
            switch (inputState)
            {
            case SHELL_INPUT_NORMAL:

                if (rx == GOS_SHELL_ESCAPE_CHAR)
                {
                    inputState = SHELL_INPUT_ESC;
                    continue;
                }

                break;

            case SHELL_INPUT_ESC:

                if (rx == '[' || rx == 'O')
                {
                    inputState = SHELL_INPUT_CSI;
                }
                else if (rx == GOS_SHELL_ESCAPE_CHAR)
                {
                    inputState = SHELL_INPUT_ESC;
                }
                else
                {
                    inputState = SHELL_INPUT_NORMAL;
                }

                continue;

            case SHELL_INPUT_CSI:

                if ((u8_t)rx < 0x40u || rx == '[')
                {
                    continue;
                }

                inputState = SHELL_INPUT_NORMAL;

                if (rx == 'A')
                {
                    /* Up Arrow */
                    if (historyCount > 0u && historyCursor < historyCount)
                    {
                        historyCursor++;
                        const char_t* historyLine = gos_shellHistoryGet(historyCursor);

                        if (historyLine != NULL)
                        {
                            (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                            (void_t) strncpy(commandBuffer, historyLine, sizeof(commandBuffer) - 1u);

                            commandBufferLength = (u16_t) strlen(commandBuffer);
                            commandBufferIndex  = commandBufferLength;

                            gos_shellRefreshCommandLine();
                        }
                    }
                }
                else if (rx == 'C')
                {
                    /* Right Arrow */
                    if (commandBufferIndex < commandBufferLength)
                    {
                        commandBufferIndex++;

                        if (useEcho == GOS_TRUE)
                        {
                            (void_t) gos_shellDriverTransmitString("\x1B[C");
                        }
                    }
                }
                else if (rx == 'D')
                {
                    /* Left Arrow */
                    if (commandBufferIndex > 0u)
                    {
                        commandBufferIndex--;

                        if (useEcho == GOS_TRUE)
                        {
                            (void_t) gos_shellDriverTransmitString("\x1B[D");
                        }
                    }
                }
                else if (rx == 'B')
                {
                    /* Down Arrow */
                    if (historyCursor > 0u)
                    {
                        historyCursor--;

                        if (historyCursor == 0u)
                        {
                            (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                            commandBufferLength = 0u;
                            commandBufferIndex  = 0u;
                            gos_shellRefreshCommandLine();
                        }
                        else
                        {
                            const char_t* historyLine = gos_shellHistoryGet(historyCursor);

                            if (historyLine != NULL)
                            {
                                (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                                (void_t) strncpy(commandBuffer, historyLine, sizeof(commandBuffer) - 1u);

                                commandBufferLength = (u16_t) strlen(commandBuffer);
                                commandBufferIndex  = commandBufferLength;

                                gos_shellRefreshCommandLine();
                            }
                        }
                    }
                }

                continue;
            }

            if (rx == '\b' || rx == 0x7F)
            {
                if (commandBufferIndex > 0u)
                {
                    u16_t tailLen = commandBufferLength - commandBufferIndex;
                    commandBufferIndex--;

                    if (tailLen > 0u)
                    {
                        (void_t) memmove(&commandBuffer[commandBufferIndex],
                                        &commandBuffer[commandBufferIndex + 1u],
                                        tailLen);
                        commandBufferLength--;
                        commandBuffer[commandBufferLength] = '\0';

                        if (useEcho == GOS_TRUE)
                        {
                            gos_shellHistoryResetBrowse();
                            gos_shellRefreshCommandLine();
                        }
                    }
                    else
                    {
                        commandBufferLength--;
                        commandBuffer[commandBufferLength] = '\0';

                        if (useEcho == GOS_TRUE)
                        {
                            gos_shellHistoryResetBrowse();
                            (void_t) gos_shellDriverTransmitString("\b \b");
                        }
                    }
                }
            }
            else if (rx == '\r' || rx == '\n')
            {
                if (cmdInputState == SHELL_CMD_INPUT_PASSWORD)
                {
                    if (useEcho == GOS_TRUE)
                    {
                        (void_t) gos_shellDriverTransmitString("\r\n");
                    }

                    /* First line: username */
                    if (currentUser.username[0] == '\0')
                    {
                        (void_t) strncpy(currentUser.username, commandBuffer, GOS_SHELL_USERNAME_MAX_LEN - 1u);
                        currentUser.username[GOS_SHELL_USERNAME_MAX_LEN - 1u] = '\0';

                        (void_t) gos_shellDriverTransmitString("Password: ");
                        gos_shellPasswordInputStart(GOS_TRUE); /* password hidden */
                    }
                    /* Second line: password */
                    else
                    {
                        gos_time_t systime;

                        if (gos_shellAuthenticateUser(currentUser.username, commandBuffer) == GOS_TRUE)
                        {
                            isAuthenticated = GOS_TRUE;
                            (void_t) gos_timeGet(&systime);
                            (void_t) gos_shellDriverTransmitString(
                                "Login successful at %4u-%02u-%02u %02u:%02u:%02u.%03u. Welcome %s!\r\n",
                                systime.years, systime.months, systime.days,
                                systime.hours, systime.minutes, systime.seconds,
                                systime.milliseconds, currentUser.username);
                        }
                        else
                        {
                            isAuthenticated = GOS_FALSE;
                            (void_t) gos_shellDriverTransmitString("Login failed. Invalid username or password.\r\n");
                            (void_t) memset(&currentUser, 0, sizeof(gos_shellUser_t));
                        }

                        gos_shellPasswordInputEnd();
                        commandBufferIndex = 0u;
                        commandBufferLength = 0u;
                        (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                        (void_t) gos_shellDriverTransmitString("\r%s", GOS_SHELL_DISPLAY_TEXT);
                    }
                }
                else
                {
                    /* Normal command input */
                    if (useEcho == GOS_TRUE)
                    {
                        (void_t) gos_shellDriverTransmitString("\r\n");
                    }

                    commandBuffer[commandBufferLength] = '\0';

                    /* Save previous command safely */
                    (void_t) memset(previousCommand, 0, sizeof(previousCommand));
                    (void_t) strncpy(previousCommand, commandBuffer, sizeof(previousCommand) - 1u);

                    /* Parse command and parameters using a separate buffer index */
                    u16_t bufIdx = 0u;
                    u16_t cmdLen = 0u;
                    paramIndex = 0u;

                    /* Extract command (bounded) */
                    while (commandBuffer[bufIdx] != ' ' && commandBuffer[bufIdx] != '\0' &&
                       cmdLen < (CFG_SHELL_MAX_COMMAND_LENGTH - 1u))
                    {
                        actualCommand[cmdLen++] = commandBuffer[bufIdx++];
                    }
                    actualCommand[cmdLen] = '\0';

                    /* Skip single space separator if present */
                    if (commandBuffer[bufIdx] == ' ')
                    {
                        bufIdx++;
                    }

                    /* Extract parameters (bounded) */
                    while (commandBuffer[bufIdx] != '\0' && paramIndex < (CFG_SHELL_MAX_PARAMS_LENGTH - 1u))
                    {
                        commandParams[paramIndex++] = commandBuffer[bufIdx++];
                    }
                    commandParams[paramIndex] = '\0';

                    if (actualCommand[0] != '\0')
                    {
                        gos_shellHistoryPush(commandBuffer);

                        /* Check if command is login or logout (always available) */
                        if (strcmp(actualCommand, "login") == 0)
                        {
                            gos_shellLoginCommand(commandParams);
                        }
                        else if (strcmp(actualCommand, "logout") == 0)
                        {
                            gos_shellLogoutCommand(commandParams);
                        }
                        else if (isAuthenticated == GOS_FALSE)
                        {
                            (void_t) gos_shellDriverTransmitString("Access denied. Please login first.\r\n");
                        }
                        else
                        {
                            /* Search for command with auth check */
                            for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
                            {
                                if (shellCommands[index].commandHandler != NULL &&
                                    strcmp(shellCommands[index].command, actualCommand) == 0)
                                {
                                    /* Check if user has required privilege */
                                    if (currentUser.privilege >= shellCommands[index].commandPrivilege)
                                    {
                                        (void_t) gos_taskSetPrivileges(shellDaemonTaskId, shellCommands[index].commandHandlerPrivileges);
                                        shellCommands[index].commandHandler(commandParams);
                                        (void_t) gos_taskSetPrivileges(shellDaemonTaskId, GOS_TASK_PRIVILEGE_KERNEL);
                                    }
                                    else
                                    {
                                        (void_t) gos_shellDriverTransmitString("Permission denied. Insufficient privileges.\r\n");
                                    }
                                    break;
                                }
                            }

                            if (index == CFG_SHELL_MAX_COMMAND_NUMBER)
                            {
                                (void_t) gos_shellDriverTransmitString("Unrecognized command!\r\n");
                            }
                        }
                    }

                    commandBufferIndex = 0u;
                    commandBufferLength = 0u;
                    gos_shellHistoryResetBrowse();
                    (void_t) memset((void_t*)commandBuffer, '\0', CFG_SHELL_COMMAND_BUFFER_SIZE);

                    // Only print prompt if not entering password mode.
                    if (cmdInputState == SHELL_CMD_INPUT_NORMAL)
                    {
                        (void_t) gos_shellDriverTransmitString("\r%s", GOS_SHELL_DISPLAY_TEXT);
                    }
                }
            }
            else
            {
                if (commandBufferLength < (CFG_SHELL_COMMAND_BUFFER_SIZE - 1u))
                {
                    if (commandBufferIndex < commandBufferLength)
                    {
                        u16_t tailLen = commandBufferLength - commandBufferIndex;

                        (void_t) memmove(&commandBuffer[commandBufferIndex + 1u],
                                        &commandBuffer[commandBufferIndex],
                                        tailLen);

                        commandBuffer[commandBufferIndex] = rx;
                        commandBufferIndex++;
                        commandBufferLength++;
                        commandBuffer[commandBufferLength] = '\0';

                        if (useEcho == GOS_TRUE && isPasswordInputMode == GOS_FALSE)
                        {
                            gos_shellRefreshCommandLine();
                        }
                    }
                    else
                    {
                        commandBuffer[commandBufferIndex++] = rx;
                        commandBufferLength++;
                        commandBuffer[commandBufferIndex] = '\0';

                        if (useEcho == GOS_TRUE && isPasswordInputMode == GOS_FALSE)
                        {
                            (void_t) gos_shellDriverTransmitString("%c", rx);
                        }
                    }
                }
                else
                {
                    (void_t) memset((void_t*)commandBuffer, '\0', CFG_SHELL_COMMAND_BUFFER_SIZE);
                    commandBufferIndex = 0u;
                    commandBufferLength = 0u;
                    gos_shellHistoryResetBrowse();
                }
            }
        }
        //(void_t) gos_taskSleep(GOS_SHELL_DAEMON_POLL_TIME_MS);
    }
}

/**
 * @brief   Shell command handler.
 * @details Handles the built-in shell command.
 *
 * @return    -
 */
GOS_STATIC void_t gos_shellCommandHandler (char_t* params)
{
    /* Check authentication for shell commands */
    if (gos_shellCanExecuteCommand(GOS_SHELL_USER_PRIVILEGE_ROOT) == GOS_FALSE)
    {
        (void_t) gos_shellDriverTransmitString("Access denied. Please login.\r\n");
        return;
    }

    /*
     * Local variables.
     */
    u8_t                    index        = 0u;
    gos_shellCommandIndex_t commandIndex = 0u;
    gos_tid_t               taskId       = GOS_INVALID_TASK_ID;
    gos_runtime_t           totalRunTime = {0};

    /*
     * Function code.
     */
    if (strcmp(params, "dump") == 0)
    {
        gos_Dump();
    }
    else if (strcmp(params, "reset") == 0)
    {
        gos_kernelReset();
    }
    else if (strcmp(params, "help") == 0)
    {
        (void_t) gos_shellDriverTransmitString("List of registered shell commands: \r\n");
        for (commandIndex = 0u; commandIndex < CFG_SHELL_MAX_COMMAND_NUMBER; commandIndex++)
        {
            /* Only print registered commands */
            if (shellCommands[commandIndex].commandHandler == NULL)
            {
                break;
            }
            else if (strcmp(shellCommands[commandIndex].command, "shell") == 0)
            {
                (void_t) gos_shellDriverTransmitString("\t"
                        "- shell\r\n\t\t"
                        "- dump\r\n\t\t"
                        "- reset\r\n\t\t"
                        "- delete_tid\r\n\t\t"
                        "- suspend_tid\r\n\t\t"
                        "- resume_tid\r\n\t\t"
                        "- unblock_tid\r\n\t\t"
                        "- delete\r\n\t\t"
                        "- suspend\r\n\t\t"
                        "- resume\r\n\t\t"
                        "- unblock\r\n\t\t"
                        "- runtime\r\n\t\t"
                        "- cpu\r\n");
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("\t- %s\r\n", shellCommands[commandIndex].command);
            }
        }
    }
    else
    {
        while (params[index] != ' ' && params[index])
        {
            index++;
        }

        params[index] = '\0';

        if (strcmp(params, "delete_tid") == 0)
        {
            taskId = (gos_tid_t)strtol(&params[++index], NULL, 16);

            if (gos_taskDelete(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("0x%X task has been deleted.\r\n", taskId);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("0x%X task could not be deleted.\r\n", taskId);
            }
        }
        else if (strcmp(params, "delete") == 0)
        {
            if (gos_taskGetId(&params[++index], &taskId) == GOS_SUCCESS)
            {
                if (gos_taskDelete(taskId) == GOS_SUCCESS)
                {
                    (void_t) gos_shellDriverTransmitString("%s has been deleted.\r\n", &params[index]);
                }
                else
                {
                    (void_t) gos_shellDriverTransmitString("%s could not be deleted.\r\n", &params[index]);
                }
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
            }
        }
        else if (strcmp(params, "suspend_tid") == 0)
        {
            taskId = (gos_tid_t)strtol(&params[++index], NULL, 16);

            if (gos_taskSuspend(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("0x%X task has been suspended.\r\n", taskId);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("0x%X task could not be suspended.\r\n", taskId);
            }
        }
        else if (strcmp(params, "suspend") == 0)
        {
            if (gos_taskGetId(&params[++index], &taskId) == GOS_SUCCESS)
            {
                if (gos_taskSuspend(taskId) == GOS_SUCCESS)
                {
                    (void_t) gos_shellDriverTransmitString("%s has been suspended.\r\n", &params[index]);
                }
                else
                {
                    (void_t) gos_shellDriverTransmitString("%s could not be suspended.\r\n", &params[index]);
                }
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
            }
        }
        else if (strcmp(params, "resume_tid") == 0)
        {
            taskId = (gos_tid_t)strtol(&params[++index], NULL, 16);

            if (gos_taskResume(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("0x%X task has been resumed.\r\n", taskId);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("0x%X task could not be resumed.\r\n", taskId);
            }
        }
        else if (strcmp(params, "resume") == 0)
        {
            if (gos_taskGetId(&params[++index], &taskId) == GOS_SUCCESS)
            {
                if (gos_taskResume(taskId) == GOS_SUCCESS)
                {
                    (void_t) gos_shellDriverTransmitString("%s has been resumed.\r\n", &params[index]);
                }
                else
                {
                    (void_t) gos_shellDriverTransmitString("%s could not be resumed.\r\n", &params[index]);
                }
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
            }
        }
        else if (strcmp(params, "unblock_tid") == 0)
        {
            taskId = (gos_tid_t)strtol(&params[++index], NULL, 16);

            if (gos_taskUnblock(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("0x%X task has been unblocked.\r\n", taskId);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("0x%X task could not be unblocked.\r\n", taskId);
            }
        }
        else if (strcmp(params, "unblock") == 0)
        {
            if (gos_taskGetId(&params[++index], &taskId) == GOS_SUCCESS)
            {
                if (gos_taskUnblock(taskId) == GOS_SUCCESS)
                {
                    (void_t) gos_shellDriverTransmitString("%s has been unblocked.\r\n", &params[index]);
                }
                else
                {
                    (void_t) gos_shellDriverTransmitString("%s could not be unblocked.\r\n", &params[index]);
                }
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
            }
        }
        else if (strcmp(params, "cpu") == 0)
        {
            (void_t) gos_shellDriverTransmitString("CPU usage: %u.%02u%%\r\n", (gos_kernelGetCpuUsage() / 100), (gos_kernelGetCpuUsage() % 100));
        }
        else if (strcmp(params, "runtime") == 0)
        {
            (void_t) gos_runTimeGet(&totalRunTime);

            (void_t) gos_shellDriverTransmitString(
                    "System runtime: %d days %d hours %d minutes %d seconds\r\n",
                    totalRunTime.days,
                    totalRunTime.hours,
                    totalRunTime.minutes,
                    totalRunTime.seconds
                    );
        }
        else
        {
            // Nothing to do.
        }
    }
}

/**
 * Register an application user
 */
gos_result_t gos_shellRegisterUser(const char_t* username, const char_t* password, gos_shellUserPrivilege_t privilege)
{
    if (registeredUsersCount >= GOS_SHELL_MAX_USERS || username == NULL || password == NULL)
    {
        return GOS_ERROR;
    }

    (void_t) strncpy(registeredUsers[registeredUsersCount].username, username, GOS_SHELL_USERNAME_MAX_LEN - 1u);
    (void_t) strncpy(registeredUsers[registeredUsersCount].password, password, GOS_SHELL_PASSWORD_MAX_LEN - 1u);
    registeredUsers[registeredUsersCount].privilege = privilege;
    registeredUsers[registeredUsersCount].isEnabled = GOS_TRUE;
    registeredUsersCount++;

    return GOS_SUCCESS;
}

/**
 * Authenticate user
 */
GOS_STATIC bool_t gos_shellAuthenticateUser (GOS_CONST char_t* username, GOS_CONST char_t* password)
{
    u16_t index = 0u;

    /* Try root user first */
    if (strcmp(username, rootUser.username) == 0 && strcmp(password, rootUser.password) == 0)
    {
        (void_t) memcpy(&currentUser, &rootUser, sizeof(gos_shellUser_t));
        return GOS_TRUE;
    }

    /* Try registered users */
    for (index = 0u; index < registeredUsersCount; index++)
    {
        if (registeredUsers[index].isEnabled &&
            strcmp(username, registeredUsers[index].username) == 0 &&
            strcmp(password, registeredUsers[index].password) == 0)
        {
            (void_t) memcpy(&currentUser, &registeredUsers[index], sizeof(gos_shellUser_t));
            return GOS_TRUE;
        }
    }

    return GOS_FALSE;
}

/**
 * Check if current user can execute command
 */
GOS_STATIC bool_t gos_shellCanExecuteCommand (gos_shellUserPrivilege_t requiredPrivilege)
{
    if (isAuthenticated == GOS_FALSE)
    {
        return GOS_FALSE;
    }

    return currentUser.privilege >= requiredPrivilege;
}

/**
 * Begin credential input mode.
 */
GOS_STATIC void_t gos_shellPasswordInputStart (bool_t hideInput)
{
    cmdInputState = SHELL_CMD_INPUT_PASSWORD;
    isPasswordInputMode = hideInput;
    (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
    commandBufferIndex = 0u;
    commandBufferLength = 0u;
}

/**
 * End credential input mode.
 */
GOS_STATIC void_t gos_shellPasswordInputEnd (void_t)
{
    isPasswordInputMode = GOS_FALSE;
    cmdInputState = SHELL_CMD_INPUT_NORMAL;
}

/**
 * Updated login command handler
 */
GOS_STATIC void_t gos_shellLoginCommand (char_t* params)
{
    (void_t) params;

    if (isAuthenticated == GOS_TRUE)
    {
        (void_t) gos_shellDriverTransmitString("Already logged in. Please logout first.\r\n");
        return;
    }

    (void_t) memset(&currentUser, 0, sizeof(gos_shellUser_t));
    (void_t) gos_shellDriverTransmitString("Username: ");
    gos_shellPasswordInputStart(GOS_FALSE); /* username visible */
}

/**
 * Updated logout command handler
 */
GOS_STATIC void_t gos_shellLogoutCommand (char_t* params)
{
    (void_t) params;

    if (isAuthenticated == GOS_TRUE)
    {
        (void_t) gos_shellDriverTransmitString("Logged out.\r\n");
        isAuthenticated = GOS_FALSE;
        (void_t) memset(&currentUser, 0, sizeof(gos_shellUser_t));
    }
    else
    {
        (void_t) gos_shellDriverTransmitString("Not logged in.\r\n");
    }
}
