#ifndef IPROGRAMVERSION_HPP_
#define IPROGRAMVERSION_HPP_

/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ProgramVersion.hpp - Signature of the firmware version string.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2008-2011 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 */

#include "Global.hpp"

#if __cplusplus
extern "C"
{
#endif

/** the maximum length of a version string */
#define VERSION_SIZE 256

    /**
     * mandatory parameter to define: Program name string
     */
    extern const char* PROGRAM_SHORT_NAME;

    /**
     * mandatory parameter to define: Program name string
     */
    extern const char* PROGRAM_NAME;

    /** mandatory parameter to define: major version string */
    extern const char* const PROGRAM_MAJOR_VERSION;

    /** mandatory parameter to define: minor version string */
    extern const char* const PROGRAM_MINOR_VERSION;

    /** mandatory parameter to define: CVS revision string */
    extern const char* const PROGRAM_REVISION;

    /** mandatory parameter to define: the copyright */
    extern const char* const PROGRAM_COPYRIGHT;

    /** mandatory parameter to define: optional information */
    extern const char* const PROGRAM_INFO;

    /** build date */
    extern const char* const PROGRAM_BUILD_DATE;

    /** Version control time stamp */
    extern const char* const PROGRAM_VCS_DATE;

    /**
     * Provides the Program name, copyright and version information as
     * readable formated string.
     *
     * @returns
     *      a pointer on a static version string.
     *
     * @ingroup main
     */
    extern const char*
    getVersionString();

    /**
     * Returns the program version as a number.
     * "01.02.03" becomes 10203.
     *
     * @return
     *      the major version and minor version numbers in integer format.
     */
    extern int32_t
    getVersionNumber();

#if __cplusplus
} // end extern C
#endif

#endif /* IPROGRAMVERSION_HPP_ */
