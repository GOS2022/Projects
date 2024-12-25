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
//! @date       2024-06-27
//! @version    1.0
//!
//! @brief      GOS2022 Library / Service Layer Configuration header.
//! @details    This file contains the configurable parameters of the SVL.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-06-27    Ahmed Gazar     Initial version created.
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
#ifndef SVL_CONFIG_H
#define SVL_CONFIG_H

/*
 * Macros
 */
/*
 * DHS Service Configuration
 */
/**
 * TODO
 */
#define SVL_DHS_MAX_DEVICES            ( 8u )

/**
 * TODO
 */
#define SVL_DHS_DAEMON_PRIO            ( 200 )

/*
 * FRS Service Configuration
 */
/**
 * TODO
 */
#define SVL_FRS_FAULT_DATA_SIZE        ( 64u )

/**
 * TODO
 */
#define SVL_FRS_ENTRY_NUM_ADDR         ( 0u  )

/**
 * TODO
 */
#define SVL_FRS_ENTRY_START_ADDR       ( SVL_FRS_ENTRY_NUM_ADDR + sizeof(u32_t) )

/**
 * TODO
 */
#define SVL_FRS_ENTRY_SIZE             ( sizeof(svl_frsFault_t) )

/**
 * TODO
 */
#define SVL_FRS_MUTEX_TMO_MS           ( 2000u )

/*
 * HWI Service Configuration
 */
/**
 * TODO
 */
#define SVL_HWI_CRC_ADDR               ( 0u  )

/**
 * TODO
 */
#define SVL_HWI_ENTRY_START_ADDR       ( SVL_HWI_CRC_ADDR + sizeof(u32_t) )

/**
 * TODO
 */
#define SVL_HWI_ENTRY_SIZE             ( sizeof(svl_hwiInfo_t) )

/**
 * TODO
 */
#define SVL_HWI_BOARD_NAME_LENGTH      ( 32u )

/**
 * TODO
 */
#define SVL_HWI_REVISION_LENGTH        ( 8u  )

/**
 * TODO
 */
#define SVL_HWI_AUTHOR_LENGTH          ( 32u )

/**
 * TODO
 */
#define SVL_HWI_DESCRIPTION_LENGTH     ( 64u )

/**
 * TODO
 */
#define SVL_HWI_SERIAL_NUMBER_LENGTH   ( 64u )

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
#define SVL_IPL_RX_BUFF_SIZE                 ( 128 )

//! Maximum number of user-defined message callbacks.
#define SVL_IPL_USER_MSG_CALLBACK_MAX_NUM    ( 8 )

/**
 * Trace level for IPL debugging.
 *
 * 0 : Debug messages disabled.
 * 1 : Only state machine state info.
 * 2 : All debug info.
 */
#define SVL_IPL_TRACE_LEVEL                  ( 1 )

//! IPL daemon task priority.
#define SVL_IPL_DAEMON_PRIORITY              ( 190 )

//! IPL daemon task stack size.
#define SVL_IPL_DAEMON_STACK_SIZE            ( 0x400 )

/*
 * SDH Service Configuration
 */
/**
 * Trace level for IPL debugging.
 *
 * 0 : Debug messages disabled.
 * 1 : Only state machine state info.
 * 2 : All debug info.
 */
#define SVL_SDH_TRACE_LEVEL                  ( 2 )

//! SDH daemon task priority.
#define SVL_SDH_DAEMON_PRIORITY              ( 189 )

//! SDH daemon task stack size.
#define SVL_SDH_DAEMON_STACK_SIZE            ( 0x400 )

/*
 * DSM Service Configuration
 */
/**
 * TODO
 */
#define SVL_DSM_MAX_INITIALIZERS       ( 32u )

/**
 * TODO
 */
#define SVL_DSM_DAEMON_PRIO            ( 200 )

/**
 * TODO
 */
#define SVL_DSM_DAEMON_POLL_MS         ( 20u )


#endif
