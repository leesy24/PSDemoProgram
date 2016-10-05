/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       Global.hpp - Common project header.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2014 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 */

#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

/*
 * Include our own stdint.h.
 * MinGW and arm-none-eabi unfortunately define different types of
 * standard integers ("typedef signed int int32_t" vs "typedef int int32_t"
 * and so on). This lead to a number of problems when dealing with
 * formats and unit tests.
 */
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#define __need_wchar_t
#include <stdint.h>
#include <stddef.h>
#include <limits>

/** a byte is defined as unsigned 8 bit value */
typedef unsigned char byte_t;

/** a character is defined as a signed 8 bit value */
typedef signed char char_t;

/**
 * Definition of a standard ASCII string to store 61 characters.
 * Used to store file names and formated numbers.
 * The last character is reserved for NULL.
 */
typedef char string16_t[16];

/**
 * Definition of a standard ASCII string to store 31 characters.
 * The last character is reserved for NULL.
 */
typedef char string32_t[32];

/**
 * Definition of a standard ASCII string to store 127 characters.
 * Used to store formatted lines of text for terminal output.
 * The last character is reserved for NULL.
 */
typedef char string128_t[128];

/**
 * Definition of a standard ASCII string to store 1023 characters.
 * Used to store formatted lines of text for terminal output.
 * The last character is reserved for NULL.
 */
typedef char string1024_t[1024];

/**
 * Typesafe cast without ugly reinterpret_cast.
 */
union cast_ptr_t
{
        /** as void pointer. Can be initialized with any address */
        void* asVoidPtr;

        /** as pointer on a char buffer */
        char* asCharPtr;

        /** as pointer on an integer buffer */
        int32_t* asIntegerPtr;

        /** as pointer on an unsigned buffer */
        uint32_t* asUnsignedPtr;
};

/** Macro to convert raw distances to meter */
#define DIST_TO_METER(d)  (0.0001 * (d))

/** Macro to convert raw time stamps to seconds */
#define TIME_TO_SECONDS(t) (0.001 * (t))

#endif /* GLOBAL_HPP_ */
