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
//! @file       svl_cfg.h
//! @author     Ahmed Gazar
//! @date       2025-01-30
//! @version    1.1
//!
//! @brief      GOS2022 Library / Service Layer Configuration header.
//! @details    This file contains the configurable parameters of the SVL.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-06-27    Ahmed Gazar     Initial version created.
// 1.1        2025-01-29    Ahmed Gazar     +    ERS and MDI parameters added
//                                          -    FRS parameters removed
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
#ifndef SVL_CONFIG_BOOTLOADER_H
#define SVL_CONFIG_BOOTLOADER_H

/*
 * Macros
 */
/*
 * DHS Service Configuration
 */
//! TODO
#define SVL_DHS_MAX_DEVICES                  ( 2u )

//! TODO
#define SVL_DHS_DAEMON_PRIO                  ( 200 )

/*
 * ERS Service Configuration
 */
//! Event description length.
#define SVL_ERS_DESC_LENGTH                  ( 64u )

//! Event data size.
#define SVL_ERS_DATA_SIZE                    ( 8u )

//! ERS mutex timeout in [ms].
#define SVL_ERS_MUTEX_TMO_MS                 ( 2000u )

//! Address of ERS entries number.
#define SVL_ERS_ENTRY_NUM_ADDR               ( 0u  )

//! Start address of ERS entries.
#define SVL_ERS_ENTRY_START_ADDR             ( SVL_ERS_ENTRY_NUM_ADDR + sizeof(u32_t) )

/*
 * IPL Service Configuration
 */

//! WiFi SSID maximum length.
#define SVL_IPL_DEVCFG_WIFI_SSID_MAX_LENGTH  ( 32 )

//! WiFi password maximum length.
#define SVL_IPL_DEVCFG_WIFI_PWD_MAX_LENGTH   ( 32 )

//! IP address element number.
#define SVL_IPL_DEVCFG_IP_ADDR_ELEMENTS      ( 4 )

//! Gateway address element number.
#define SVL_IPL_DEVCFG_GATEWAY_ADDR_ELEMENTS ( 4 )

//! Sub-net address element number.
#define SVL_IPL_DEVCFG_SUBNET_ADDR_ELEMENTS  ( 4 )

//! IPL RX buffer size.
#define SVL_IPL_RX_BUFF_SIZE                 ( 4096u )

//! Maximum number of user-defined message callbacks.
#define SVL_IPL_USER_MSG_CALLBACK_MAX_NUM    ( 16 )

/**
 * Trace level for IPL debugging.
 *
 * 0 : Debug messages disabled.
 * 1 : Only state machine state info.
 * 2 : All debug info.
 */
#define SVL_IPL_TRACE_LEVEL                  ( 0 )

//! IPL daemon task priority.
#define SVL_IPL_DAEMON_PRIORITY              ( 190 )

//! IPL daemon task stack size.
#define SVL_IPL_DAEMON_STACK_SIZE            ( 0x800 )

/*
 * SDH Service Configuration
 */
/**
 * Trace level for SDH debugging.
 *
 * 0 : Debug messages disabled.
 * 1 : Only state machine state info.
 * 2 : All debug info.
 */
#define SVL_SDH_TRACE_LEVEL                  ( 2 )

//! SDH daemon task priority.
#define SVL_SDH_DAEMON_PRIORITY              ( 189 )

//! SDH daemon task stack size.
#define SVL_SDH_DAEMON_STACK_SIZE            ( 0x800 )

/*
 * DSM Service Configuration
 */
//! TODO
#define SVL_DSM_MAX_BLOCK_NAME_LENGTH        ( 24u )

//! TODO
#define SVL_DSM_MAX_PHASE_NAME_LENGTH        ( 64u )

//! TODO
#define SVL_DSM_MAX_INITIALIZERS             ( 16u )

//! TODO
#define SVL_DSM_DAEMON_PRIO                  ( 200 )

//! TODO
#define SVL_DSM_DAEMON_POLL_MS               ( 20u )

//! TODO
#define SVL_DSM_DAEMON_STACK_SIZE            ( 0x600u )

/*
 * MDI Service Configuration
 */
//! TODO
#define SVL_MDI_VAR_NAME_LENGTH              ( 16u )

#endif
