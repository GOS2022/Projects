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
//! @file       bld_cfg.h
//! @author     Ahmed Gazar
//! @date       2024-02-07
//! @version    1.0
//!
//! @brief      GOS2022 Library / Bootloader configuration header.
//! @details    TODO
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-02-07    Ahmed Gazar     Initial version created.
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
#ifndef BLD_CFG_H
#define BLD_CFG_H

/*
 * Includes
 */
#include <gos.h>

/*
 * Macros
 */
#define BLD_ROM_START_ADDRESS          ( (u32_t)0x08004000   )
#define BLD_ROM_SIZE                   ( (u32_t)(112 * 1024)  )
#define BLD_ROM_END_ADDRESS            ( BLD_ROM_START_ADDRESS + BLD_ROM_SIZE - 1 )

#define BLD_APP_ROM_START_ADDRESS      ( (u32_t)0x08020000   )
#define BLD_APP_ROM_SIZE               ( (u32_t)(384 * 1024) )
#define BLD_APP_ROM_END_ADDRESS        ( BLD_APP_ROM_START_ADDRESS + BLD_APP_ROM_SIZE - 1 )

#define BLD_DEFAULT_CONN_TMO_MS        ( 3000u               )
#define BLD_DEFAULT_REQ_TMO_MS         ( 60000u              )
#define BLD_DEFAULT_INSTALL_TMO_MS     ( 120000u             )
#define BLD_DEFAULT_CONN_ON_STARTUP    ( GOS_FALSE            )

#endif
