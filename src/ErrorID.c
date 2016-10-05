/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ErrorID.c
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2014 Triple-IN GmbH Hamburg, Germany
 *
 * All rights reserved. Using, copying, publishing
 * or distributing  is not permitted without prior
 * written agreement of Triple-IN GmbH.
 ****************************************************************************
 */
#include "ErrorID.h"

/*
 * Convert an integer into ErrorID_t.
 */
ErrorID_t
intToError(long theErrorID)
{
    // preset: not a valid error ID.
    long index = ERR_INVALID_HANDLE - __ERR_LASTERROR;

    ErrorID_t lErrorID[] = { //
            ERR_SUCCESS, /* OK */
            ERR_SYSTEM_NOT_READY, /* system not ready */
            ERR_FRONT_SCREEN_NOT_CLEAR, /* front screen dirty or wet */
            ERR_TEMPERATURE_OUT_OF_RANGE, /* temperature out of operating range */
            ERR_KEM_SCAN_UNIT_FAILURE, /* Angle encoder or motor unit failure. */
            ERR_KEM_UNIT,/* KEM unit / KEM measurement ufailure */
            ERR_SERIAL_VERSION, /* Error in serialization: version does not match. */
            ERR_CONFIGURATION_ERROR, /** < invalid or defect system configuration */
            ERR_FATAL_SYSTEM_ERROR, /* Fatal system error */
            ERR_BUFFER_OVERFLOW, /* internal Buffer overflow;  */
            ERR_INDEX_OUT_OF_RANGE, /* array index is out of range */
            ERR_DIV_BY_ZERO, /* Division by zero */
            ERR_INVALID_HANDLE, /* Invalid handle / Bad address */
            ERR_UNSUPPORTED_FUNCTION, /* Function not supported */
            ERR_ACCESS_DENIED, /* Access denied / Permission denied */
            ERR_INVALID_PARAMETER, /* Parameter is out of range */
            ERR_UNKNOWN_COMMAND, /* Unknown command */
            ERR_CRC, /* CRC checksum error */
            ERR_USER_BREAK, /* user break*/
            ERR_TIMEOUT, /* Timeout expired */
            ERR_WRITE, /* cannot write */
            ERR_READ, /* cannot read */
            ERR_IO /* physical I/O error */
            };

    // not a negative error code
    if (0 <= theErrorID)
    {
        index = 0;
    }

    // convert error ID into array index
    else if ((__ERR_LASTERROR < theErrorID) && (-__ELASTERROR >= theErrorID))
    {
        index = theErrorID - __ERR_LASTERROR;
    }

    return lErrorID[index];
}
