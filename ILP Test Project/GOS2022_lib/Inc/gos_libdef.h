/*
 * libdef.h
 *
 *  Created on: Feb 6, 2024
 *      Author: Gabor
 */

#ifndef GOS_LIBDEF_H
#define GOS_LIBDEF_H

/*
 * Includes
 */
#include <gos.h>
#include <string.h>

/*
 * Macros
 */
/**
 * Library major version.
 */
#define LIB_VERSION_MAJOR      ( 0                 )

/**
 * Library minor version.
 */
#define LIB_VERSION_MINOR      ( 10                )

/**
 * Library build number.
 */
#define LIB_VERSION_BUILD      ( 0                 )

/**
 * Library version date / year.
 */
#define LIB_VERSION_DATE_YEAR  ( 2024              )

/**
 * Library version date / month.
 */
#define LIB_VERSION_DATE_MONTH ( GOS_TIME_JULY     )

/**
 * Library version date / day.
 */
#define LIB_VERSION_DATE_DAY   ( 19                )

/**
 * Library name.
 */
#define LIB_NAME               "GOS2022_lib"

/**
 * Library description.
 */
#define LIB_DESCRIPTION        "GOS2022 device driver and service package."

/**
 * Library author.
 */
#define LIB_AUTHOR             "Ahmed Ibrahim Gazar"

/**
 * Library name length.
 */
#define LIB_NAME_LENGTH        ( 48 )

/**
 * Library description length.
 */
#define LIB_DESCRIPTION_LENGTH ( 64 )

/**
 * Library author length.
 */
#define LIB_AUTHOR_LENGTH      ( 32 )

/*
 * Type definitions
 */
/**
 * Library version type.
 */
typedef struct
{
	u16_t 	   major;
	u16_t	   minor;
	u16_t 	   build;
	gos_time_t date;
	char_t     name        [LIB_NAME_LENGTH];
	char_t     description [LIB_DESCRIPTION_LENGTH];
	char_t     author      [LIB_AUTHOR_LENGTH];
}gos_libVersion_t;

/**
 * TODO
 */
gos_result_t gos_libGetVersion (gos_libVersion_t* pLibVersion);

#endif /* LIBDEF_H_ */
