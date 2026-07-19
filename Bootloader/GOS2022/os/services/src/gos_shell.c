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
//! @date       2026-07-19
//! @version    1.13
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
// 1.12       2026-07-17    Ahmed Gazar     *    Major fixes and user handling introduced
// 1.13       2026-07-19    Ahmed Gazar     +    Shell restart task commands added
//                                               (restart_tid, restart)
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
 * Escape character used to detect VT100 control sequences.
 */
#define GOS_SHELL_ESCAPE_CHAR            ((char)0x1Bu)

/**
 * Maximum number of application-defined shell users.
 */
#define GOS_SHELL_MAX_USERS              (8u)

/**
 * Maximum shell user name length including string terminator.
 */
#define GOS_SHELL_USERNAME_MAX_LEN       (32u)

/**
 * Maximum shell password length including string terminator.
 */
#define GOS_SHELL_PASSWORD_MAX_LEN       (32u)

/**
 * History buffer size configuration.
 */
#define CFG_SHELL_HISTORY_SIZE           (8u)

/**
 * Max parsed arguments (including command token).
 */
#define CFG_SHELL_MAX_ARGUMENTS          (16u)

/*
 * Type definitions
 */
/**
 * Input states for processing special characters.
 */
typedef enum
{
    SHELL_INPUT_NORMAL, //!< Normal character processing state.
    SHELL_INPUT_ESC,    //!< Escape character received.
    SHELL_INPUT_CSI     //!< Control sequence introducer processing state.
} shellInputState_t;

/**
 * Command input mode state.
 */
typedef enum
{
    SHELL_CMD_INPUT_NORMAL,  //!< Regular command line input mode.
    SHELL_CMD_INPUT_PASSWORD //!< Credential input mode.
} shellCmdInputState_t;

/**
 * User structure.
 */
typedef struct
{
    char_t username [GOS_SHELL_USERNAME_MAX_LEN]; //!< User name.
    char_t password [GOS_SHELL_PASSWORD_MAX_LEN]; //!< User password.
    gos_shellUserPrivilege_t privilege;           //!< User shell privilege.
    bool_t isEnabled;                             //!< User enabled state.
} gos_shellUser_t;

/*
 * Static variables
 */
/**
 * Shell command array.
 */
GOS_STATIC gos_shellCommand_t  shellCommands        [CFG_SHELL_MAX_COMMAND_NUMBER];

/**
 * Shell daemon task ID.
 */
GOS_STATIC gos_tid_t           shellDaemonTaskId;

/**
 * Command buffer.
 */
GOS_STATIC char_t              commandBuffer        [CFG_SHELL_COMMAND_BUFFER_SIZE];

/**
 * Command buffer index.
 */
GOS_STATIC u16_t               commandBufferIndex;

/**
 * Command buffer length.
 */
GOS_STATIC u16_t               commandBufferLength;

/**
 * Shell echo flag.
 */
GOS_STATIC bool_t              useEcho;

/**
 * Actual command buffer.
 */
GOS_STATIC char_t              actualCommand        [CFG_SHELL_MAX_COMMAND_LENGTH];

/**
 * Previously executed command buffer.
 */
GOS_STATIC char_t              previousCommand      [CFG_SHELL_MAX_COMMAND_LENGTH];

/**
 * Current escape-sequence parser state.
 */
GOS_STATIC shellInputState_t   inputState = SHELL_INPUT_NORMAL;

/**
 * Current command input mode state.
 */
GOS_STATIC shellCmdInputState_t cmdInputState = SHELL_CMD_INPUT_NORMAL;

/**
 * Shell command history buffer.
 */
GOS_STATIC char_t               historyBuffer [CFG_SHELL_HISTORY_SIZE][CFG_SHELL_COMMAND_BUFFER_SIZE];

/**
 * Number of stored history entries.
 */
GOS_STATIC u16_t                historyCount;

/**
 * Current history browsing cursor.
 */
GOS_STATIC u16_t                historyCursor;

/**
 * Registered application users.
 */
GOS_STATIC gos_shellUser_t      registeredUsers [GOS_SHELL_MAX_USERS];

/**
 * Number of registered application users.
 */
GOS_STATIC u16_t                registeredUsersCount = 0u;

/**
 * Currently authenticated user.
 */
GOS_STATIC gos_shellUser_t      currentUser;

/**
 * Authentication state of the current shell session.
 */
GOS_STATIC bool_t               isAuthenticated = GOS_FALSE;

/**
 * Indicates whether typed characters must be hidden.
 */
GOS_STATIC bool_t               isPasswordInputMode = GOS_FALSE;

/**
 * Root user definition (always available).
 */
GOS_STATIC gos_shellUser_t      rootUser =
{
    .username  = CFG_SHELL_ROOT_USERNAME,
    .password  = CFG_SHELL_ROOT_PASSWORD,
    .privilege = GOS_SHELL_USER_PRIVILEGE_ROOT,
    .isEnabled = GOS_TRUE
};

/*
 * Function prototypes
 */
GOS_STATIC void_t            gos_shellDaemonTask         (void_t);
GOS_STATIC void_t            gos_shellRefreshCommandLine (void_t);
GOS_STATIC void_t            gos_shellHistoryInit        (void_t);
GOS_STATIC void_t            gos_shellHistoryPush        (GOS_CONST char_t* command);
GOS_STATIC GOS_CONST char_t* gos_shellHistoryGet         (u16_t cursor);
GOS_STATIC void_t            gos_shellHistoryResetBrowse (void_t);
GOS_STATIC void_t            gos_shellCommandHandler     (char_t** params);
GOS_STATIC bool_t            gos_shellAuthenticateUser   (GOS_CONST char_t* username, GOS_CONST char_t* password);
GOS_STATIC bool_t            gos_shellCanExecuteCommand  (gos_shellUserPrivilege_t requiredPrivilege);
GOS_STATIC void_t            gos_shellPasswordInputStart (bool_t hideInput);
GOS_STATIC void_t            gos_shellPasswordInputEnd   (void_t);
GOS_STATIC void_t            gos_shellLoginCommand       (char_t** params);
GOS_STATIC void_t            gos_shellLogoutCommand      (char_t** params);
GOS_STATIC u16_t             gos_shellTokenize           (char_t* input, char_t** argv, u16_t maxArgv);

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
    .command                  = "shell",
    .commandHandler           = gos_shellCommandHandler,
    .commandHandlerPrivileges = GOS_TASK_PRIVILEGE_KERNEL,
    .commandPrivilege         = GOS_SHELL_USER_PRIVILEGE_ROOT
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

    // Initialize entire shellCommands array and all buffers to safe defaults.
    (void_t) memset(shellCommands,   0, sizeof(shellCommands));
    (void_t) memset(commandBuffer,   0, sizeof(commandBuffer));
    (void_t) memset(actualCommand,   0, sizeof(actualCommand));
    (void_t) memset(previousCommand, 0, sizeof(previousCommand));
    (void_t) memset(&currentUser,    0, sizeof(gos_shellUser_t));

    commandBufferIndex = 0u;
    commandBufferLength = 0u;

    inputState = SHELL_INPUT_NORMAL;

    gos_shellHistoryInit();

    for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
    {
        // Already zeroed above, but keep setting handler explicitly for clarity.
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
    if ((command != NULL) && (command->commandHandler != NULL))
    {
        for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
        {
            if (shellCommands[index].commandHandler == NULL)
            {
                shellCommands[index].commandHandler           = command->commandHandler;
                shellCommands[index].commandHandlerPrivileges = command->commandHandlerPrivileges;
                shellCommands[index].commandPrivilege         = command->commandPrivilege;
                (void_t) strncpy(shellCommands[index].command, command->command, sizeof(shellCommands[index].command) - 1u);
                shellCommands[index].command[sizeof(shellCommands[index].command) - 1u] = '\0';
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

/**
 * Register an application user
 */
gos_result_t gos_shellRegisterUser (GOS_CONST char_t* username, GOS_CONST char_t* password, gos_shellUserPrivilege_t privilege)
{
    /*
     * Local variables.
     */
    gos_result_t registerResult;

    /*
     * Function code.
     */
    if ((registeredUsersCount >= GOS_SHELL_MAX_USERS) || (username == NULL) || (password == NULL))
    {
        registerResult = GOS_ERROR;
    }
    else
    {
        (void_t) strncpy(registeredUsers[registeredUsersCount].username, username, GOS_SHELL_USERNAME_MAX_LEN - 1u);
        registeredUsers[registeredUsersCount].username[GOS_SHELL_USERNAME_MAX_LEN - 1u] = '\0';
        (void_t) strncpy(registeredUsers[registeredUsersCount].password, password, GOS_SHELL_PASSWORD_MAX_LEN - 1u);
        registeredUsers[registeredUsersCount].password[GOS_SHELL_PASSWORD_MAX_LEN - 1u] = '\0';

        registeredUsers[registeredUsersCount].privilege = privilege;
        registeredUsers[registeredUsersCount].isEnabled = GOS_TRUE;
        registeredUsersCount++;

        registerResult = GOS_SUCCESS;
    }

    return registerResult;
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

/*
 * Function: gos_shellForcePrintPrompt
 */
gos_result_t gos_shellForcePrintPrompt (void_t)
{
    /*
     * Function code.
     */
	return gos_shellDriverTransmitString("\r%s", GOS_SHELL_DISPLAY_TEXT);
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
    gos_shellCommandIndex_t index = 0u;
    gos_time_t              systime;
    char_t                  rx;
    GOS_CONST char_t*       historyLine;
    char_t*                 argv [CFG_SHELL_MAX_ARGUMENTS + 1u] = {0};
    u16_t                   argc;

    /*
     * Function code.
     */
    (void_t) gos_taskSleep(CFG_SHELL_STARTUP_DELAY_MS);
    (void_t) gos_shellDriverTransmitString(GOS_SHELL_DISPLAY_TEXT);

    for (;;)
    {
        rx = '\0';
        argc = 0u;

        if (gos_shellDriverReceiveChar(&rx) == GOS_SUCCESS)
        {
            switch (inputState)
            {
                case SHELL_INPUT_NORMAL:
                {
                    if (rx == GOS_SHELL_ESCAPE_CHAR)
                    {
                        inputState = SHELL_INPUT_ESC;
                        continue;
                    }
                    else
                    {
                        // Nothing to do.
                    }

                    break;
                }
                case SHELL_INPUT_ESC:
                {
                    if ((rx == '[') || (rx == 'O'))
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
                }
                case SHELL_INPUT_CSI:
                {
                    if (((u8_t)rx < 0x40u) || (rx == '['))
                    {
                        continue;
                    }
                    else
                    {
                        // Nothing to do.
                    }

                    inputState = SHELL_INPUT_NORMAL;

                    if (rx == 'A')
                    {
                        // Up arrow.
                        if ((historyCount > 0u) && (historyCursor < historyCount))
                        {
                            historyCursor++;
                            historyLine = gos_shellHistoryGet(historyCursor);

                            if (historyLine != NULL)
                            {
                                (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                                (void_t) strncpy(commandBuffer, historyLine, sizeof(commandBuffer) - 1u);

                                commandBufferLength = (u16_t) strlen(commandBuffer);
                                commandBufferIndex  = commandBufferLength;

                                gos_shellRefreshCommandLine();
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
                    else if (rx == 'C')
                    {
                        // Right arrow.
                        if (commandBufferIndex < commandBufferLength)
                        {
                            commandBufferIndex++;

                            if (useEcho == GOS_TRUE)
                            {
                                (void_t) gos_shellDriverTransmitString("\x1B[C");
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
                    else if (rx == 'D')
                    {
                        // Left arrow.
                        if (commandBufferIndex > 0u)
                        {
                            commandBufferIndex--;

                            if (useEcho == GOS_TRUE)
                            {
                                (void_t) gos_shellDriverTransmitString("\x1B[D");
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
                    else if (rx == 'B')
                    {
                        // Down arrow.
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
                                historyLine = gos_shellHistoryGet(historyCursor);

                                if (historyLine != NULL)
                                {
                                    (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
                                    (void_t) strncpy(commandBuffer, historyLine, sizeof(commandBuffer) - 1u);

                                    commandBufferLength = (u16_t) strlen(commandBuffer);
                                    commandBufferIndex  = commandBufferLength;

                                    gos_shellRefreshCommandLine();
                                }
                                else
                                {
                                    // Nothing to do.
                                }
                            }
                        }
                        else
                        {
                            // Nothing to do.
                        }
                    }

                    continue;
                }
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
                        else
                        {
                            // Nothing to do.
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
                        else
                        {
                            // Nothing to do.
                        }
                    }
                }
                else
                {
                    // Nothing to do.
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
                    else
                    {
                        // Nothing to do.
                    }

                    // First line: user name.
                    if (currentUser.username[0] == '\0')
                    {
                        (void_t) strncpy(currentUser.username, commandBuffer, GOS_SHELL_USERNAME_MAX_LEN - 1u);
                        currentUser.username[GOS_SHELL_USERNAME_MAX_LEN - 1u] = '\0';

                        (void_t) gos_shellDriverTransmitString("Password: ");
                        // Hide password.
                        gos_shellPasswordInputStart(GOS_TRUE);
                    }
                    // Second line: password.
                    else
                    {
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
                    if (useEcho == GOS_TRUE)
                    {
                        (void_t) gos_shellDriverTransmitString("\r\n");
                    }
                    else
                    {
                        // Nothing to do.
                    }

                    commandBuffer[commandBufferLength] = '\0';

                    (void_t) memset(previousCommand, 0, sizeof(previousCommand));
                    (void_t) strncpy(previousCommand, commandBuffer, sizeof(previousCommand) - 1u);

                    if (commandBuffer[0] != '\0')
                    {
                        gos_shellHistoryPush(commandBuffer);
                        argc = gos_shellTokenize(commandBuffer, argv, CFG_SHELL_MAX_ARGUMENTS + 1u);

                        if (argc > 0u)
                        {
                            (void_t) memset(actualCommand, 0, sizeof(actualCommand));
                            (void_t) strncpy(actualCommand, argv[0], sizeof(actualCommand) - 1u);

                            if (strcmp(actualCommand, "login") == 0)
                            {
                                gos_shellLoginCommand((argc > 1u) ? &argv[1] : NULL);
                            }
                            else if (strcmp(actualCommand, "logout") == 0)
                            {
                                gos_shellLogoutCommand((argc > 1u) ? &argv[1] : NULL);
                            }
                            else if (isAuthenticated == GOS_FALSE)
                            {
                                (void_t) gos_shellDriverTransmitString("Access denied. Please login first.\r\n");
                            }
                            else
                            {
                                for (index = 0u; index < CFG_SHELL_MAX_COMMAND_NUMBER; index++)
                                {
                                    if (shellCommands[index].commandHandler != NULL &&
                                        strcmp(shellCommands[index].command, actualCommand) == 0)
                                    {
                                        if (currentUser.privilege >= shellCommands[index].commandPrivilege)
                                        {
                                            (void_t) gos_taskSetPrivileges(shellDaemonTaskId, shellCommands[index].commandHandlerPrivileges);
                                            shellCommands[index].commandHandler((argc > 1u) ? &argv[1] : NULL);
                                            (void_t) gos_taskSetPrivileges(shellDaemonTaskId, GOS_TASK_PRIVILEGE_KERNEL);
                                        }
                                        else
                                        {
                                            (void_t) gos_shellDriverTransmitString("Permission denied. Insufficient privileges.\r\n");
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        // Nothing to do.
                                    }
                                }

                                if (index == CFG_SHELL_MAX_COMMAND_NUMBER)
                                {
                                    (void_t) gos_shellDriverTransmitString("Unrecognized command!\r\n");
                                }
                                else
                                {
                                    // Nothing to do.
                                }
                            }
                        }
                    }

                    commandBufferIndex = 0u;
                    commandBufferLength = 0u;
                    gos_shellHistoryResetBrowse();
                    (void_t) memset((void_t*)commandBuffer, '\0', CFG_SHELL_COMMAND_BUFFER_SIZE);

                    if (cmdInputState == SHELL_CMD_INPUT_NORMAL)
                    {
                        (void_t) gos_shellDriverTransmitString("\r%s", GOS_SHELL_DISPLAY_TEXT);
                    }
                    else
                    {
                        // Nothing to do.
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
                        else
                        {
                            // Nothing to do.
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
                        else
                        {
                            // Nothing to do.
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
    }
}

/**
 * @brief   Refresh shell command line display.
 * @details Reprints the prompt and current command buffer and restores the cursor
 *          position when line editing is active.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellRefreshCommandLine (void_t)
{
    /*
     * Function code.
     */
    if (useEcho == GOS_TRUE)
    {
        (void_t) gos_shellDriverTransmitString("\r%s%s\x1B[K", GOS_SHELL_DISPLAY_TEXT, commandBuffer);

        if (commandBufferIndex < commandBufferLength)
        {
            (void_t) gos_shellDriverTransmitString("\x1B[%uD", (u32_t)(commandBufferLength - commandBufferIndex));
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

/**
 * @brief   Initialize shell history handling.
 * @details Clears history counters and resets history browsing state.
 *
 * @return  -
 */
GOS_STATIC GOS_INLINE void_t gos_shellHistoryInit (void_t)
{
    /*
     * Function code.
     */
    historyCount  = 0u;
    historyCursor = 0u;
}

/**
 * @brief   Push a command into shell history.
 * @details Stores a non-empty command in the history buffer while avoiding
 *          consecutive duplicates.
 *
 * @param   command  Command string to store.
 *
 * @return  -
 */
GOS_STATIC GOS_INLINE void_t gos_shellHistoryPush (GOS_CONST char_t* command)
{
    /*
     * Function code.
     */
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

/**
 * @brief   Get a history entry by browse cursor.
 * @details Returns a pointer to the command corresponding to the current
 *          history browse position.
 *
 * @param   cursor  History browse cursor value.
 *
 * @return  Pointer to the stored command, or NULL if cursor is invalid.
 */
GOS_STATIC GOS_INLINE GOS_CONST char_t* gos_shellHistoryGet (u16_t cursor)
{
    /*
     * Function code.
     */
    if (cursor == 0u || cursor > historyCount)
    {
        return NULL;
    }

    return historyBuffer[historyCount - cursor];
}

/**
 * @brief   Reset history browsing state.
 * @details Moves the history cursor back to the live command line.
 *
 * @return  -
 */
GOS_STATIC GOS_INLINE void_t gos_shellHistoryResetBrowse (void_t)
{
    /*
     * Function code.
     */
    historyCursor = 0u;
}

/**
 * @brief   Shell command handler.
 * @details Handles the built-in shell command and its subcommands.
 *
 * @param   params  NULL-terminated parameter list.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellCommandHandler (char_t** params)
{
    /*
     * Local variables.
     */
    gos_shellCommandIndex_t commandIndex = 0u;
    gos_tid_t               taskId       = GOS_INVALID_TASK_ID;
    gos_runtime_t           totalRunTime = {0};

    /*
     * Function code.
     */
    if (gos_shellCanExecuteCommand(GOS_SHELL_USER_PRIVILEGE_ROOT) == GOS_FALSE)
    {
        (void_t) gos_shellDriverTransmitString("Access denied. Please login.\r\n");
        return;
    }

    if ((params == NULL) || (params[0] == NULL))
    {
        return;
    }

    if (strcmp(params[0], "dump") == 0)
    {
        gos_Dump();
    }
    else if (strcmp(params[0], "reset") == 0)
    {
        gos_kernelReset();
    }
    else if (strcmp(params[0], "help") == 0)
    {
        (void_t) gos_shellDriverTransmitString("List of registered shell commands: \r\n");
        for (commandIndex = 0u; commandIndex < CFG_SHELL_MAX_COMMAND_NUMBER; commandIndex++)
        {
            // Only print registered commands.
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
                        "- restart_tid\r\n\t\t"
                        "- unblock_tid\r\n\t\t"
                        "- delete\r\n\t\t"
                        "- suspend\r\n\t\t"
                        "- resume\r\n\t\t"
                        "- restart\r\n\t\t"
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
    else if (strcmp(params[0], "delete_tid") == 0 && params[1] != NULL)
    {
        taskId = (gos_tid_t)strtol(params[1], NULL, 16);

        if (gos_taskDelete(taskId) == GOS_SUCCESS)
        {
            (void_t) gos_shellDriverTransmitString("0x%X task has been deleted.\r\n", taskId);
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("0x%X task could not be deleted.\r\n", taskId);
        }
    }
    else if (strcmp(params[0], "delete") == 0 && params[1] != NULL)
    {
        if (gos_taskGetId(params[1], &taskId) == GOS_SUCCESS)
        {
            if (gos_taskDelete(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("%s has been deleted.\r\n", params[1]);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("%s could not be deleted.\r\n", params[1]);
            }
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
        }
    }
    else if (strcmp(params[0], "suspend_tid") == 0 && params[1] != NULL)
    {
        taskId = (gos_tid_t)strtol(params[1], NULL, 16);

        if (gos_taskSuspend(taskId) == GOS_SUCCESS)
        {
            (void_t) gos_shellDriverTransmitString("0x%X task has been suspended.\r\n", taskId);
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("0x%X task could not be suspended.\r\n", taskId);
        }
    }
    else if (strcmp(params[0], "suspend") == 0 && params[1] != NULL)
    {
        if (gos_taskGetId(params[1], &taskId) == GOS_SUCCESS)
        {
            if (gos_taskSuspend(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("%s has been suspended.\r\n", params[1]);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("%s could not be suspended.\r\n", params[1]);
            }
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
        }
    }
    else if (strcmp(params[0], "resume_tid") == 0 && params[1] != NULL)
    {
        taskId = (gos_tid_t)strtol(params[1], NULL, 16);

        if (gos_taskResume(taskId) == GOS_SUCCESS)
        {
            (void_t) gos_shellDriverTransmitString("0x%X task has been resumed.\r\n", taskId);
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("0x%X task could not be resumed.\r\n", taskId);
        }
    }
    else if (strcmp(params[0], "resume") == 0 && params[1] != NULL)
    {
        if (gos_taskGetId(params[1], &taskId) == GOS_SUCCESS)
        {
            if (gos_taskResume(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("%s has been resumed.\r\n", params[1]);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("%s could not be resumed.\r\n", params[1]);
            }
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
        }
    }
    else if (strcmp(params[0], "restart_tid") == 0 && params[1] != NULL)
    {
        taskId = (gos_tid_t)strtol(params[1], NULL, 16);

        if (gos_taskRestart(taskId) == GOS_SUCCESS)
        {
            (void_t) gos_shellDriverTransmitString("0x%X task has been restarted.\r\n", taskId);
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("0x%X task could not be restarted.\r\n", taskId);
        }
    }
    else if (strcmp(params[0], "restart") == 0 && params[1] != NULL)
    {
        if (gos_taskGetId(params[1], &taskId) == GOS_SUCCESS)
        {
            if (gos_taskRestart(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("%s has been restarted.\r\n", params[1]);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("%s could not be restarted.\r\n", params[1]);
            }
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
        }
    }
    else if (strcmp(params[0], "unblock_tid") == 0 && params[1] != NULL)
    {
        taskId = (gos_tid_t)strtol(params[1], NULL, 16);

        if (gos_taskUnblock(taskId) == GOS_SUCCESS)
        {
            (void_t) gos_shellDriverTransmitString("0x%X task has been unblocked.\r\n", taskId);
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("0x%X task could not be unblocked.\r\n", taskId);
        }
    }
    else if (strcmp(params[0], "unblock") == 0 && params[1] != NULL)
    {
        if (gos_taskGetId(params[1], &taskId) == GOS_SUCCESS)
        {
            if (gos_taskUnblock(taskId) == GOS_SUCCESS)
            {
                (void_t) gos_shellDriverTransmitString("%s has been unblocked.\r\n", params[1]);
            }
            else
            {
                (void_t) gos_shellDriverTransmitString("%s could not be unblocked.\r\n", params[1]);
            }
        }
        else
        {
            (void_t) gos_shellDriverTransmitString("Task could not be found.\r\n");
        }
    }
    else if (strcmp(params[0], "cpu") == 0)
    {
        (void_t) gos_shellDriverTransmitString("CPU usage: %u.%02u%%\r\n", (gos_kernelGetCpuUsage() / 100), (gos_kernelGetCpuUsage() % 100));
    }
    else if (strcmp(params[0], "runtime") == 0)
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

/**
 * @brief   Authenticate a shell user.
 * @details Checks the root user first, then the registered application users.
 *
 * @param   username  User name to authenticate.
 * @param   password  Password to authenticate.
 *
 * @return  GOS_TRUE if authentication succeeded, otherwise GOS_FALSE.
 */
GOS_STATIC bool_t gos_shellAuthenticateUser (GOS_CONST char_t* username, GOS_CONST char_t* password)
{
    /*
     * Local variables.
     */
    u16_t index = 0u;

    /*
     * Function code.
     */
    // Try root user first.
    if (strcmp(username, rootUser.username) == 0 && strcmp(password, rootUser.password) == 0)
    {
        (void_t) memcpy(&currentUser, &rootUser, sizeof(gos_shellUser_t));
        return GOS_TRUE;
    }
    else
    {
        // Nothing to do.
    }

    // Try registered users.
    for (index = 0u; index < registeredUsersCount; index++)
    {
        if (registeredUsers[index].isEnabled &&
            strcmp(username, registeredUsers[index].username) == 0 &&
            strcmp(password, registeredUsers[index].password) == 0)
        {
            (void_t) memcpy(&currentUser, &registeredUsers[index], sizeof(gos_shellUser_t));
            return GOS_TRUE;
        }
        else
        {
            // Nothing to do.
        }
    }

    return GOS_FALSE;
}

/**
 * @brief   Check command execution permission.
 * @details Verifies authentication state and compares user privilege with the
 *          required privilege level.
 *
 * @param   requiredPrivilege  Required shell privilege for the command.
 *
 * @return  GOS_TRUE if the command may be executed, otherwise GOS_FALSE.
 */
GOS_STATIC bool_t gos_shellCanExecuteCommand (gos_shellUserPrivilege_t requiredPrivilege)
{
    /*
     * Function code.
     */
    if (isAuthenticated == GOS_FALSE)
    {
        return GOS_FALSE;
    }
    else
    {
        // Nothing to do.
    }

    return currentUser.privilege >= requiredPrivilege;
}

/**
 * @brief   Start credential input mode.
 * @details Switches the shell to credential input mode and optionally hides
 *          entered characters.
 *
 * @param   hideInput  GOS_TRUE to suppress input echo, GOS_FALSE otherwise.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellPasswordInputStart (bool_t hideInput)
{
    /*
     * Function code.
     */
    cmdInputState = SHELL_CMD_INPUT_PASSWORD;
    isPasswordInputMode = hideInput;
    (void_t) memset(commandBuffer, 0, sizeof(commandBuffer));
    commandBufferIndex = 0u;
    commandBufferLength = 0u;
}

/**
 * @brief   End credential input mode.
 * @details Restores normal shell command input processing.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellPasswordInputEnd (void_t)
{
    /*
     * Function code.
     */
    isPasswordInputMode = GOS_FALSE;
    cmdInputState = SHELL_CMD_INPUT_NORMAL;
}

/**
 * @brief   Handle login command.
 * @details Starts the interactive user name and password input flow.
 *
 * @param   params  NULL-terminated parameter list.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellLoginCommand (char_t** params)
{
    /*
     * Function code.
     */
    (void_t) params;

    if (isAuthenticated == GOS_TRUE)
    {
        (void_t) gos_shellDriverTransmitString("Already logged in. Please logout first.\r\n");
        return;
    }
    else
    {
        // Nothing to do.
    }

    (void_t) memset(&currentUser, 0, sizeof(gos_shellUser_t));
    (void_t) gos_shellDriverTransmitString("Username: ");
    gos_shellPasswordInputStart(GOS_FALSE); /* username visible */
}

/**
 * @brief   Handle logout command.
 * @details Clears the current authentication state and active user data.
 *
 * @param   params  NULL-terminated parameter list.
 *
 * @return  -
 */
GOS_STATIC void_t gos_shellLogoutCommand (char_t** params)
{
    /*
     * Function code.
     */
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

/**
 * @brief   Tokenize a command line in-place.
 * @details Splits the input string into whitespace-separated tokens and stores
 *          pointers to them in the provided argument vector.
 *
 * @param   input    Command line buffer to tokenize.
 * @param   argv     Output argument vector.
 * @param   maxArgv  Maximum number of entries available in argv.
 *
 * @return  Number of parsed arguments.
 */
GOS_STATIC u16_t gos_shellTokenize (char_t* input, char_t** argv, u16_t maxArgv)
{
    /*
     * Local variables.
     */
    u16_t   argc = 0u;
    char_t* p    = input;

    /*
     * Function code.
     */
    if ((input == NULL) || (argv == NULL) || (maxArgv < 2u))
    {
        return 0u;
    }
    else
    {
        // Nothing to do.
    }

    while ((*p != '\0') && (argc < (maxArgv - 1u)))
    {
        // Skip separators.
        while ((*p == ' ') || (*p == '\t'))
        {
            p++;
        }

        if (*p == '\0')
        {
            break;
        }
        else
        {
            // Nothing to do.
        }

        // Start of token.
        argv[argc++] = p;

        // Find token end.
        while ((*p != '\0') && (*p != ' ') && (*p != '\t'))
        {
            p++;
        }

        if (*p == '\0')
        {
            break;
        }
        else
        {
            // Nothing to do.
        }

        // Terminate token and continue.
        *p = '\0';
        p++;
    }

    argv[argc] = NULL;
    return argc;
}
