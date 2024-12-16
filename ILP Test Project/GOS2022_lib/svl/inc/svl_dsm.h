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
//! @details    This component TODO
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
 * TODO
 */
typedef struct
{
    char_t* description;
    gos_result_t (*pInitializer)(void_t);
}svl_dsmInitBlock_t;

typedef struct
{
	char_t* phaseName;
	GOS_CONST svl_dsmInitBlock_t initBlock [SVL_DSM_MAX_INITIALIZERS];
}svl_dsmInitPhaseDesc_t;

typedef enum
{
	DSM_STATE_STARTUP,
	DSM_STATE_STARTUP_READY,
	DSM_STATE_NORMAL,
	DSM_STATE_CRITICAL,
	DSM_STATE_FATAL,
	DSM_STATE_REDUCED,
	DSM_STATE_MAINTENANCE
}svl_dsmState_t;

typedef struct
{
	svl_dsmState_t prevState;
	svl_dsmState_t currState;
	void_t (*pReaction) (void_t);
}svl_dsmReaction_t;

gos_result_t svl_dsmInit (void_t);

gos_result_t svl_dsmPrintOSInfo (void_t);

gos_result_t svl_dsmPrintLibInfo (void_t);

gos_result_t svl_dsmPrintHwInfo (void_t);

void_t svl_dsmWaitForState (svl_dsmState_t requiredState);

void_t svl_dsmSetState (svl_dsmState_t requiredState);

#endif
