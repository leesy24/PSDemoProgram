/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ProgramVersion.cpp -
 *              Program version information.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2016/01/15 10:35:56 $
 * SDK:         GNU gcc 4
 *
 * (c) 2012 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 */
#include <cstring>
#include "Global.hpp"
#include "ProgramVersion.hpp"

/** Short program name */
const char* PROGRAM_SHORT_NAME = "PSDemoProgram";

/** Program names */
const char* PROGRAM_NAME = "PSDemoProgram";

/**
 * Current version string.
 * Begin with V3 to match the firmware version.
 */
const char* const PROGRAM_MAJOR_VERSION = "03.03.";
const char* const PROGRAM_MINOR_VERSION = "01";

/** Revision string, here the release date */
const char* const PROGRAM_REVISION = "2016-01-15"; //

/** the copyright */
const char* const PROGRAM_COPYRIGHT = "(c) Triple-IN GmbH 2016";

/** optional information */
const char* const PROGRAM_INFO = ""; //

/** build date */
const char* const PROGRAM_BUILD_DATE = __DATE__ " " __TIME__;

/** Version control time stamp. Void date if release is not tagged in CVS */
const char* const PROGRAM_VCS_DATE = "$Date: 2016/01/15 10:35:56 $";

/*
 * Provides the Program name, copyright and version information as
 * readable formated string.
 *
 * @ingroup main
 */
const char*
getVersionString()
{
    static string1024_t sVersion = { 0 };

    // fill the static version string only when called first
    if (0 == sVersion[0])
    {
        strcpy(sVersion, PROGRAM_NAME);
        strcat(sVersion, "\r\n[");
        strcat(sVersion, PROGRAM_SHORT_NAME);
        strcat(sVersion, "; ");
        strcat(sVersion, PROGRAM_MAJOR_VERSION);
        strcat(sVersion, PROGRAM_MINOR_VERSION);
        strcat(sVersion, "; ");
        strcat(sVersion, PROGRAM_REVISION);
        strcat(sVersion, "; ");
        strcat(sVersion, PROGRAM_COPYRIGHT);
        strcat(sVersion, "]\r\nBuild: ");
        strcat(sVersion, PROGRAM_BUILD_DATE);
        strcat(sVersion, "\r\nInfo:   ");
        strcat(sVersion, PROGRAM_VCS_DATE);
        strcat(sVersion, "; ");
        strcat(sVersion, PROGRAM_INFO);
        strcat(sVersion, "\r\n");
    }
    return sVersion;
}

/*
 * Returns the program version as a number.
 */
int32_t
getVersionNumber()
{
    const int32_t lVersion = (PROGRAM_MAJOR_VERSION[0] - '0') * 100000 //
    + (PROGRAM_MAJOR_VERSION[1] - '0') * 10000 //
    + (PROGRAM_MAJOR_VERSION[3] - '0') * 1000 //
    + (PROGRAM_MAJOR_VERSION[4] - '0') * 100 //
    + (PROGRAM_MINOR_VERSION[0] - '0') * 10 //
    + (PROGRAM_MINOR_VERSION[1] - '0') * 1;
    return lVersion;
}

