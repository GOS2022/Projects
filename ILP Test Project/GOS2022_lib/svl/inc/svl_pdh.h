// Project Data Handler.

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
#define PDH_LIBVER_VERSION_MINOR      ( 12                )

/**
 * Library build number.
 */
#define PDH_LIBVER_VERSION_BUILD      ( 0                 )

/**
 * Library version date / year.
 */
#define PDH_LIBVER_VERSION_DATE_YEAR  ( 2024              )

/**
 * Library version date / month.
 */
#define PDH_LIBVER_VERSION_DATE_MONTH ( GOS_TIME_DECEMBER )

/**
 * Library version date / day.
 */
#define PDH_LIBVER_VERSION_DATE_DAY   ( 24                )
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
#define PDH_STRING_LENGTH       ( 48u )

/**
 * IP address number of elements (IPv4).
 */
#define PDH_IP_ADDR_ELEMENT_NUM ( 4u )


/*
 * Address definitions for individually accessing structures.
 */
// Start relative offset.
#define PDH_REL_START_ADDR    ( 0u )

// svl_pdhSwInfo_t - swInfo
#define PDH_ADDR_SW_INFO      ( PDH_REL_START_ADDR )
#define PDH_ADDR_BLD_LIB_VER  ( PDH_ADDR_SW_INFO   )
#define PDH_ADDR_BLD_SW_VER   ( PDH_ADDR_BLD_LIB_VER  + sizeof(svl_pdhSwVerInfo_t)  )
#define PDH_ADDR_BLD_OS_INFO  ( PDH_ADDR_BLD_SW_VER   + sizeof(svl_pdhSwVerInfo_t)  )
#define PDH_ADDR_BLD_BIN_INFO ( PDH_ADDR_BLD_OS_INFO  + sizeof(svl_pdhOsInfo_t)     )
#define PDH_ADDR_APP_LIB_VER  ( PDH_ADDR_BLD_BIN_INFO + sizeof(svl_pdhBinaryInfo_t) )
#define PDH_ADDR_APP_SW_VER   ( PDH_ADDR_APP_LIB_VER  + sizeof(svl_pdhSwVerInfo_t)  )
#define PDH_ADDR_APP_OS_INFO  ( PDH_ADDR_APP_SW_VER   + sizeof(svl_pdhSwVerInfo_t)  )
#define PDH_ADDR_APP_BIN_INFO ( PDH_ADDR_APP_OS_INFO  + sizeof(svl_pdhOsInfo_t)     )

// svl_pdhHwInfo_t - hwInfo
#define PDH_ADDR_HW_INFO      ( PDH_ADDR_APP_BIN_INFO + sizeof(svl_pdhBinaryInfo_t) )

// svl_pdhBldCfg_t - bldCfg
#define PDH_ADDR_BLD_CFG      ( PDH_ADDR_HW_INFO      + sizeof(svl_pdhHwInfo_t)     )

// svl_pdhWifiCfg_t - wifiCfg
#define PDH_ADDR_WIFI_CFG     ( PDH_ADDR_BLD_CFG      + sizeof(svl_pdhWifiCfg_t)    )

// User-defined.
#define PDH_ADDR_USER_START   ( PDH_ADDR_WIFI_CFG     + sizeof(svl_pdhWifiCfg_t)    )

/*
 * Type definitions
 */
/**
 * PDH read/write function type.
 */
typedef gos_result_t (*svl_pdhReadWriteFunc_t)(u32_t address, u8_t* pData, u16_t size);

/**
 * PDH configuration structure.
 */
typedef struct
{
	svl_pdhReadWriteFunc_t readFunction;
	svl_pdhReadWriteFunc_t writeFunction;
}svl_pdhCfg_t;

/**
 * PDH software version information structure.
 */
typedef struct __attribute__((packed))
{
	u16_t 	            major;
	u16_t	            minor;
	u16_t 	            build;
	gos_time_t          date;
	char_t              name        [PDH_STRING_LENGTH];
	char_t              description [PDH_STRING_LENGTH];
	char_t              author      [PDH_STRING_LENGTH];
}svl_pdhSwVerInfo_t;

/**
 * PDH OS information structure.
 */
typedef struct __attribute__((packed))
{
	u16_t 	            major;
	u16_t	            minor;
}svl_pdhOsInfo_t;

/**
 * PDH binary information structure.
 */
typedef struct __attribute__((packed))
{
	u32_t               startAddress;
	u32_t               size;
	u32_t               crc;
}svl_pdhBinaryInfo_t;

/**
 * PDH software information structure.
 */
typedef struct __attribute__((packed))
{
	svl_pdhSwVerInfo_t  bldLibVerInfo;
	svl_pdhSwVerInfo_t  bldSwVerInfo;
	svl_pdhOsInfo_t     bldOsInfo;
	svl_pdhBinaryInfo_t bldBinaryInfo;
	svl_pdhSwVerInfo_t  appLibVerInfo;
	svl_pdhSwVerInfo_t  appSwVerInfo;
	svl_pdhOsInfo_t     appOsInfo;
	svl_pdhBinaryInfo_t appBinaryInfo;
}svl_pdhSwInfo_t;

/**
 * PDH hardware information structure.
 */
typedef struct __attribute__((packed))
{
	char_t              boardName    [PDH_STRING_LENGTH];
	char_t              revision     [PDH_STRING_LENGTH];
	char_t              author       [PDH_STRING_LENGTH];
	char_t              description  [PDH_STRING_LENGTH];
	gos_time_t          date;
	char_t              serialNumber [PDH_STRING_LENGTH];
}svl_pdhHwInfo_t;

/**
 * PDH bootloader configuration structure.
 */
typedef struct __attribute__((packed))
{
	u32_t               bldTimeoutMs;
	bool_t              updateMode;
	bool_t              wirelessUpdate;
	bool_t              waitForConnectionOnStartup;
	u8_t                startupCounter;
	u32_t               connectionTimeout;
	u32_t               requestTimeout;
	u32_t               installTimeout;
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
	svl_pdhSwInfo_t     swInfo;
	svl_pdhHwInfo_t     hwInfo;
	svl_pdhBldCfg_t     bldCfg;
	svl_pdhWifiCfg_t    wifiCfg;
}svl_pdhProjData_t;


// TODO
gos_result_t svl_pdhInit (void_t);
// TODO
gos_result_t svl_pdhConfigure (svl_pdhCfg_t* pCfg);
// TODO
gos_result_t svl_pdhGetLibVersion (svl_pdhSwVerInfo_t* pCfg);
// TODO
gos_result_t svl_pdhGetSwInfo (svl_pdhSwInfo_t* pSwInfo);
// TODO
gos_result_t svl_pdhGetHwInfo (svl_pdhHwInfo_t* pHwInfo);
// TODO
gos_result_t svl_pdhGetBldCfg (svl_pdhBldCfg_t* pBldCfg);
// TODO
gos_result_t svl_pdhGetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg);
// TODO
gos_result_t svl_pdhSetSwInfo (svl_pdhSwInfo_t* pSwInfo);
// TODO
gos_result_t svl_pdhSetHwInfo (svl_pdhHwInfo_t* pHwInfo);
// TODO
gos_result_t svl_pdhSetBldCfg (svl_pdhBldCfg_t* pBldCfg);
// TODO
gos_result_t svl_pdhSetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg);

#endif
