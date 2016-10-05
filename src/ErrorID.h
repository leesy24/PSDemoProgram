#ifndef ERRORID_H_
#define ERRORID_H_

/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ErrorID.h - Error identification numbers.
 *
 * Author:      CBruelle
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2007 - 2010 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 *
 ****************************************************************************
 */

#ifndef __ELASTERROR
#define __ELASTERROR 2000
#endif

/**
 * ErrorID.h  Error identification numbers.
 * <p>&nbsp;<p>
 *
 * Typedef ErrorID_t - Error identification numbers.
 * Enumeration of errors for input/output devices and for internal errors.
 * <ul>
 * <li>
 * 0 and positive numbers means always "no error", negative indicates
 * problems.
 * </li>
 * <li>
 * Supported ARM and GCC compilers defines different numbers in <errno.h>,
 * so we decided to define some own, project depending definitions.
 * </li>
 * </ul>
 * <p>&nbsp;<p>
 *
 * Note:
 * <p>
 * <code>errno.h</code> defines <code>__ELASTERROR</code>, which is
 * used as a base for KEM-defined error values.
 * <p>
 * ANSI C requires a function <code>strerror</code>, but does not
 * specify the strings used for each error number.
 * The KEM application supplies a routine named <code>_user_strerror</code>.
 * <code>_user_strerror</code> takes one argument of type <code>int</code>,
 * and returns a character pointer with a human readable error message.
 * <p>&nbsp;<p>
 *
 * @ingroup include
 */
typedef enum ErrorID
{
    ERRORID_SERIAL_VERSION = 20120601, /**< Serial version ID of this table */

    ERR_SUCCESS = 0, /**< Function was successful.*/
    ERR_NONE = 0, /**< no error*/

    /**
     * Number negative error codes from tail to top.
     * Last error code is defined in stdlib's errno.h.
     */
    __ERR_LASTERROR = -__ELASTERROR - 22, /* 22 is the number of codes defined here. */

    ERR_SYSTEM_NOT_READY, /**< system not ready */
    ERR_FRONT_SCREEN_NOT_CLEAR, /**< front screen dirty or wet */
    ERR_TEMPERATURE_OUT_OF_RANGE, /**< temperature out of operating range */
    ERR_KEM_SCAN_UNIT_FAILURE, /**< Angle encoder or motor unit failure. */
    ERR_KEM_UNIT,/**< KEM unit / KEM measurement ufailure */
    ERR_SERIAL_VERSION, /**< Error in serialization: version does not match. */
    ERR_CONFIGURATION_ERROR, /** < invalid or defect system configuration */
    ERR_FATAL_SYSTEM_ERROR, /**< Fatal system error */
    ERR_BUFFER_OVERFLOW, /**< internal Buffer overflow;  */
    ERR_INDEX_OUT_OF_RANGE, /**< array index is out of range */
    ERR_DIV_BY_ZERO, /**< Division by zero */
    ERR_INVALID_HANDLE, /**< Invalid handle / Bad address */
    ERR_UNSUPPORTED_FUNCTION, /**< Function not supported */
    ERR_ACCESS_DENIED, /**< Access denied / Permission denied */
    ERR_INVALID_PARAMETER, /**< Parameter is out of range */
    ERR_UNKNOWN_COMMAND, /**< Unknown command */
    ERR_CRC, /**< CRC checksum error */
    ERR_USER_BREAK, /**< user break*/
    ERR_TIMEOUT, /**< Timeout expired */
    ERR_WRITE, /**< cannot write */
    ERR_READ, /**< cannot read */
    ERR_IO
/**< physical I/O error */
} ErrorID_t;

/**
 * Convert an integer into ErrorID_t.
 *
 * @param theErrorID
 *      an integer.
 *
 * @return
 *      The corresponding ErrorID_t value,
 *      or ERR_SUCCESS if (theErrorID >= 0)
 *      or ERR_INVALID_HANDLE if the negative code is not in range of ErrorID_t.
 */
ErrorID_t
intToError(long theErrorID);

#endif /* ERRORID_H_*/
