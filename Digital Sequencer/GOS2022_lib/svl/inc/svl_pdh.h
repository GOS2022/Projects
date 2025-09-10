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
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       svl_pdh.h
//! @author     Ahmed Gazar
//! @date       2025-01-30
//! @version    1.0
//!
//! @brief      GOS2022 Library / Project Data Handler
//! @details    This component implements the project data handling such as software info, hardware
//!             info, bootloader configuration and WiFi configuration data as well as the library
//!             version itself.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-01-30    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
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
#ifndef SVL_PDH_H
#define SVL_PDH_H
/*
 * Includes
 */
#include <gos.h>

/*
 * Macros
 */
/**
 * Library major version.
 */
#define PDH_LIBVER_VERSION_MAJOR      ( 0                 )

/**
 * Library minor version.
 */
#define PDH_LIBVER_VERSION_MINOR      ( 25                )

/**
 * Library build number.
 */
#define PDH_LIBVER_VERSION_BUILD      ( 0                 )

/**
 * Library version date / year.
 */
#define PDH_LIBVER_VERSION_DATE_YEAR  ( 2025              )

/**
 * Library version date / month.
 */
#define PDH_LIBVER_VERSION_DATE_MONTH ( GOS_TIME_JULY     )

/**
 * Library version date / day.
 */
#define PDH_LIBVER_VERSION_DATE_DAY   ( 22                )
/**
 * Library name.
 */
#define PDH_LIBVER_NAME               "GOS2022_lib"

/**
 * Library description.
 */
#define PDH_LIBVER_DESCRIPTION        "GOS2022 device driver and service package."

/**
 * Library author.
 */
#define PDH_LIBVER_AUTHOR             "Ahmed Ibrahim Gazar"

/**
 * Unified string length.
 */
#define PDH_STRING_LENGTH             ( 48u )

/**
 * IP address number of elements (IPv4).
 */
#define PDH_IP_ADDR_ELEMENT_NUM       ( 4u )


/*
 * Address definitions for individually accessable structures.
 */
/**
 * Start relative offset.
 */
#define PDH_REL_START_ADDR    ( 0u )

// svl_pdhSwInfo_t - swInfo
/**
 * Address of software info.
 */
#define PDH_ADDR_SW_INFO      ( PDH_REL_START_ADDR )

/**
 * Address of library version.
 */
#define PDH_ADDR_BLD_LIB_VER  ( PDH_ADDR_SW_INFO   )

/**
 * Address of bootloader software version.
 */
#define PDH_ADDR_BLD_SW_VER   ( PDH_ADDR_BLD_LIB_VER  + sizeof(svl_pdhSwVerInfo_t)  )

/**
 * Address of bootloader OS info.
 */
#define PDH_ADDR_BLD_OS_INFO  ( PDH_ADDR_BLD_SW_VER   + sizeof(svl_pdhSwVerInfo_t)  )

/**
 * Address of bootloader binary info.
 */
#define PDH_ADDR_BLD_BIN_INFO ( PDH_ADDR_BLD_OS_INFO  + sizeof(svl_pdhOsInfo_t)     )

/**
 * Address of application library version.
 */
#define PDH_ADDR_APP_LIB_VER  ( PDH_ADDR_BLD_BIN_INFO + sizeof(svl_pdhBinaryInfo_t) )

/**
 * Address of application software version.
 */
#define PDH_ADDR_APP_SW_VER   ( PDH_ADDR_APP_LIB_VER  + sizeof(svl_pdhSwVerInfo_t)  )

/**
 * Address of application OS info.
 */
#define PDH_ADDR_APP_OS_INFO  ( PDH_ADDR_APP_SW_VER   + sizeof(svl_pdhSwVerInfo_t)  )

/**
 * Address of application binary info.
 */
#define PDH_ADDR_APP_BIN_INFO ( PDH_ADDR_APP_OS_INFO  + sizeof(svl_pdhOsInfo_t)     )

// svl_pdhHwInfo_t - hwInfo
/**
 * Address of hardware info.
 */
#define PDH_ADDR_HW_INFO      ( PDH_ADDR_APP_BIN_INFO + sizeof(svl_pdhBinaryInfo_t) )

// svl_pdhBldCfg_t - bldCfg
/**
 * Address of bootloader configuration.
 */
#define PDH_ADDR_BLD_CFG      ( PDH_ADDR_HW_INFO      + sizeof(svl_pdhHwInfo_t)     )

// svl_pdhWifiCfg_t - wifiCfg
/**
 * Address of WiFi configuration.
 */
#define PDH_ADDR_WIFI_CFG     ( PDH_ADDR_BLD_CFG      + sizeof(svl_pdhWifiCfg_t)    )

/**
 * User-defined.
 */
#define PDH_ADDR_USER_START   ( PDH_ADDR_WIFI_CFG     + sizeof(svl_pdhWifiCfg_t)    )

/*
 * Type definitions
 */
/**
 * PDH read/write function type.
 */
typedef gos_result_t (*svl_pdhReadWriteFunc_t)(u32_t address, u8_t* pData, u32_t size);

/**
 * PDH configuration structure.
 */
typedef struct
{
	svl_pdhReadWriteFunc_t readFunction;  //!< Read function.
	svl_pdhReadWriteFunc_t writeFunction; //!< Write function.
}svl_pdhCfg_t;

/**
 * PDH software version information structure.
 */
typedef struct __attribute__((packed))
{
	u16_t 	            major;                           //!< Major version.
	u16_t	            minor;                           //!< Minor version.
	u16_t 	            build;                           //!< Build number.
	gos_time_t          date;                            //!< Creation date.
	char_t              name        [PDH_STRING_LENGTH]; //!< Name.
	char_t              description [PDH_STRING_LENGTH]; //!< Description.
	char_t              author      [PDH_STRING_LENGTH]; //!< Author.
}svl_pdhSwVerInfo_t;

/**
 * PDH OS information structure.
 */
typedef struct __attribute__((packed))
{
	u16_t 	            major; //!< Major version.
	u16_t	            minor; //!< Minor version.
}svl_pdhOsInfo_t;

/**
 * PDH binary information structure.
 */
typedef struct __attribute__((packed))
{
	u32_t               startAddress; //!< Start memory address.
	u32_t               size;         //!< Size in bytes.
	u32_t               crc;          //!< CRC32.
}svl_pdhBinaryInfo_t;

/**
 * PDH software information structure.
 */
typedef struct __attribute__((packed))
{
	svl_pdhSwVerInfo_t  bldLibVerInfo;  //!< Bootloader library info.
	svl_pdhSwVerInfo_t  bldSwVerInfo;   //!< Bootloader version info.
	svl_pdhOsInfo_t     bldOsInfo;      //!< Bootloader OS info.
	svl_pdhBinaryInfo_t bldBinaryInfo;  //!< Bootloader binary info.
	svl_pdhSwVerInfo_t  appLibVerInfo;  //!< Application library info.
	svl_pdhSwVerInfo_t  appSwVerInfo;   //!< Application version info.
	svl_pdhOsInfo_t     appOsInfo;      //!< Application OS info.
	svl_pdhBinaryInfo_t appBinaryInfo;  //!< Application binary info.
}svl_pdhSwInfo_t;

/**
 * PDH hardware information structure.
 */
typedef struct __attribute__((packed))
{
	char_t              boardName    [PDH_STRING_LENGTH];  //!< Board name.
	char_t              revision     [PDH_STRING_LENGTH];  //!< Board revision.
	char_t              author       [PDH_STRING_LENGTH];  //!< Author.
	char_t              description  [PDH_STRING_LENGTH];  //!< Description.
	gos_time_t          date;                              //!< Creation date.
	char_t              serialNumber [PDH_STRING_LENGTH];  //!< S/N.
}svl_pdhHwInfo_t;

/**
 * PDH bootloader configuration structure.
 */
typedef struct __attribute__((packed))
{
	bool_t              installRequested;           //!< Application install request flag.
	u8_t                reserved;                   //!< Reserved for future use.
	u16_t               binaryIndex;                //!< Index of binary to be installed.
	bool_t              updateMode;                 //!< Update mode flag.
	u8_t                startupCounter;             //!< Startup counter to select mode.
	u32_t               requestTimeout;             //!< Request timeout [ms].
	u32_t               installTimeout;             //!< Install timeout [ms].
}svl_pdhBldCfg_t;

/**
 * PDH WiFi configuration structure.
 */
typedef struct __attribute__((packed))
{
    char_t ssid      [PDH_STRING_LENGTH];       //!< WiFi SSID.
    char_t pwd       [PDH_STRING_LENGTH];       //!< WiFi password.
    u8_t   ipAddress [PDH_IP_ADDR_ELEMENT_NUM]; //!< IP address.
    u8_t   gateway   [PDH_IP_ADDR_ELEMENT_NUM]; //!< Gateway.
    u8_t   subnet    [PDH_IP_ADDR_ELEMENT_NUM]; //!< Sub-net.
    u16_t  port;
}svl_pdhWifiCfg_t;

/**
 * PDH project data structure.
 */
typedef struct __attribute__((packed))
{
	svl_pdhSwInfo_t     swInfo;  //!< Software info.
	svl_pdhHwInfo_t     hwInfo;  //!< Hardware info.
	svl_pdhBldCfg_t     bldCfg;  //!< Bootloader configuration.
	svl_pdhWifiCfg_t    wifiCfg; //!< WiFi configuration.
}svl_pdhProjData_t;


/**
 * @brief   Initializes the PDH.
 * @details Registers the sysmon custom messages for the service, and
 *          initializes the internal mutex.
 *
 * @return  Result of initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   Sysmon registration or mutex error.
 */
gos_result_t svl_pdhInit (void_t);

/**
 * @brief   Configures the PDH.
 * @details Stores the function pointers to the user-defined read and write functions.
 *
 * @param   pCfg Pointer to the configuration variable.
 *
 * @return  Result of PDH configuration.
 *
 * @retval  #GOS_SUCCESS Configuration successful.
 * @retval  #GOS_ERROR   NULL pointer.
 */
gos_result_t svl_pdhConfigure (svl_pdhCfg_t* pCfg);

/**
 * @brief   Gets the library version info.
 * @details Copies the library version info into the given variable.
 *
 * @param   pLibVer Pointer to a variable to store the library version in.
 *
 * @return  Result of library version info getting.
 *
 * @retval  #GOS_SUCCESS Getting successful.
 * @retval  #GOS_ERROR   NULL pointer.
 */
gos_result_t svl_pdhGetLibVersion (svl_pdhSwVerInfo_t* pLibVer);

/**
 * @brief   Gets the software info.
 * @details Reads the software info from the corresponding address.
 *
 * @param   pSwInfo Pointer to a variable to store the software info in.
 *
 * @return  Result of software info getting.
 *
 * @retval  #GOS_SUCCESS Getting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing read function or mutex error.
 */
gos_result_t svl_pdhGetSwInfo (svl_pdhSwInfo_t* pSwInfo);

/**
 * @brief   Gets the hardware info.
 * @details Reads the hardware info from the corresponding address.
 *
 * @param   pHwInfo Pointer to a variable to store the hardware info in.
 *
 * @return  Result of hardware info getting.
 *
 * @retval  #GOS_SUCCESS Getting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing read function or mutex error.
 */
gos_result_t svl_pdhGetHwInfo (svl_pdhHwInfo_t* pHwInfo);

/**
 * @brief   Gets the bootloader configuration.
 * @details Reads the bootloader configuration from the corresponding address.
 *
 * @param   pBldCfg Pointer to a variable to store the bootloader configuration in.
 *
 * @return  Result of bootloader configuration getting.
 *
 * @retval  #GOS_SUCCESS Getting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing read function or mutex error.
 */
gos_result_t svl_pdhGetBldCfg (svl_pdhBldCfg_t* pBldCfg);

/**
 * @brief   Gets the WiFi configuration.
 * @details Reads the WiFi configuration from the corresponding address.
 *
 * @param   pWifiCfg Pointer to a variable to store the WiFi configuration in.
 *
 * @return  Result of WiFi configuration getting.
 *
 * @retval  #GOS_SUCCESS Getting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing read function or mutex error.
 */
gos_result_t svl_pdhGetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg);

/**
 * @brief   Sets the software info.
 * @details Writes the software info to the corresponding address.
 *
 * @param   pSwInfo Pointer to a variable containing the desired info.
 *
 * @return  Result of software info setting.
 *
 * @retval  #GOS_SUCCESS Setting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing write function or mutex error.
 */
gos_result_t svl_pdhSetSwInfo (svl_pdhSwInfo_t* pSwInfo);

/**
 * @brief   Sets the hardware info.
 * @details Writes the hardware info to the corresponding address.
 *
 * @param   pHwInfo Pointer to a variable containing the desired info.
 *
 * @return  Result of hardware info setting.
 *
 * @retval  #GOS_SUCCESS Setting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing write function or mutex error.
 */
gos_result_t svl_pdhSetHwInfo (svl_pdhHwInfo_t* pHwInfo);

/**
 * @brief   Sets the bootloader configuration.
 * @details Writes the bootloader configuration to the corresponding address.
 *
 * @param   pBldCfg Pointer to a variable containing the desired configuration.
 *
 * @return  Result of bootloader configuration setting.
 *
 * @retval  #GOS_SUCCESS Setting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing write function or mutex error.
 */
gos_result_t svl_pdhSetBldCfg (svl_pdhBldCfg_t* pBldCfg);

/**
 * @brief   Sets the WiFi configuration.
 * @details Writes the WiFi configuration to the corresponding address.
 *
 * @param   pWifiCfg Pointer to a variable containing the desired configuration.
 *
 * @return  Result of WiFi configuration setting.
 *
 * @retval  #GOS_SUCCESS Setting successful.
 * @retval  #GOS_ERROR   NULL pointer or missing write function or mutex error.
 */
gos_result_t svl_pdhSetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg);

#endif
