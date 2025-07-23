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
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       svl_dsm.h
//! @author     Ahmed Gazar
//! @date       2024-07-16
//! @version    1.0
//!
//! @brief      GOS2022 Library / Device State Manager header.
//! @details    This component implements the execution of the startup functions, and the
//!             pre-defined reactions to state changes.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-07-16    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
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
#ifndef SVL_DSM_H
#define SVL_DSM_H

#include <gos.h>
#include <svl_cfg.h>

/**
 * DSM initializer block.
 */
typedef struct
{
    char_t*       description;            //!< Initializer description.
    gos_result_t (*pInitializer)(void_t); //!< Initializer function.
}svl_dsmInitBlock_t;

/**
 * DSM initialization phase configuration structure.
 */
typedef struct
{
	char_t*                      phaseName;                            //!< Initialization phase name.
	GOS_CONST svl_dsmInitBlock_t initBlock [SVL_DSM_MAX_INITIALIZERS]; //!< Initializer block array.
}svl_dsmInitPhaseDesc_t;

/**
 * DSM states.
 */
typedef enum
{
	DSM_STATE_STARTUP,       //!< Startup.
	DSM_STATE_STARTUP_READY, //!< Startup ready.
	DSM_STATE_NORMAL,        //!< Normal.
	DSM_STATE_CRITICAL,      //!< Critical.
	DSM_STATE_FATAL,         //!< Fatal.
	DSM_STATE_REDUCED,       //!< Reduced.
	DSM_STATE_MAINTENANCE    //!< Maintenance.
}svl_dsmState_t;

/**
 * DSM reaction descriptor structure.
 */
typedef struct
{
	svl_dsmState_t prevState;     //!< Previous state for change detection.
	svl_dsmState_t currState;     //!< Current state for change detection.
	void_t (*pReaction) (void_t); //!< Reaction to be executed.
}svl_dsmReaction_t;

/**
 * @brief   Initializes the DSM service.
 * @details TODO
 *
 * @return  Result of
 *
 * @retval  #GOS_SUCCESS
 * @retval  #GOS_ERROR
 */
gos_result_t svl_dsmInit (void_t);

/**
 * @brief   Prints the OS info.
 * @details TODO
 *
 * @return  Result of
 *
 * @retval  #GOS_SUCCESS
 * @retval  #GOS_ERROR
 */
gos_result_t svl_dsmPrintOSInfo (void_t);

/**
 * @brief   Prints the library info.
 * @details TODO
 *
 * @return  Result of
 *
 * @retval  #GOS_SUCCESS
 * @retval  #GOS_ERROR
 */
gos_result_t svl_dsmPrintLibInfo (void_t);

/**
 * @brief   Prints the hardware info.
 * @details TODO
 *
 * @return  Result of
 *
 * @retval  #GOS_SUCCESS
 * @retval  #GOS_ERROR
 */
gos_result_t svl_dsmPrintHwInfo (void_t);

/**
 * @brief   Prints the application info.
 * @details TODO
 *
 * @param
 *
 * @return  Result of
 *
 * @retval  #GOS_SUCCESS
 * @retval  #GOS_ERROR
 */
gos_result_t svl_dsmPrintAppInfo (void_t);

/**
 * @brief   Waits for a specific state.
 * @details Sends the current task to sleep until the required state
 *          has been set.
 *
 * @param   requiredState State to wait for.
 *
 * @return  -
 */
void_t svl_dsmWaitForState (svl_dsmState_t requiredState);

/**
 * @brief   Sets the required state.
 * @details Sets the previous and current states.
 *
 * @param   requiredState State to be set.
 *
 * @return  -
 */
void_t svl_dsmSetState (svl_dsmState_t requiredState);

/**
 *
 * @param
 * @return
 */
__attribute__((weak)) gos_result_t svl_dsmPlatformInit (void_t);

/**
 *
 * @param
 * @return
 */
__attribute__((weak)) gos_result_t svl_dsmApplicationInit (void_t);

#endif
