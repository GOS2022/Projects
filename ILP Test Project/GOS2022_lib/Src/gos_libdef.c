// TODO
#include <gos_libdef.h>

/*
 * Function: gos_libGetVersion
 */
gos_result_t gos_libGetVersion (gos_libVersion_t* pLibVersion)
{
    /*
     * Local variables.
     */
    gos_result_t getVersionResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pLibVersion != NULL)
    {
    	pLibVersion->major       = LIB_VERSION_MAJOR;
    	pLibVersion->minor       = LIB_VERSION_MINOR;
    	pLibVersion->build       = LIB_VERSION_BUILD;
    	pLibVersion->date.years  = LIB_VERSION_DATE_YEAR;
    	pLibVersion->date.months = LIB_VERSION_DATE_MONTH;
    	pLibVersion->date.days   = LIB_VERSION_DATE_DAY;

        (void_t) strcpy((char_t*)pLibVersion->name, LIB_NAME);
        (void_t) strcpy((char_t*)pLibVersion->description, LIB_DESCRIPTION);
        (void_t) strcpy((char_t*)pLibVersion->author, LIB_AUTHOR);

        getVersionResult = GOS_SUCCESS;
    }
    else
    {
        // Target is NULL.
    }

    return getVersionResult;
}
