#ifndef CRC32_HPP_
#define CRC32_HPP_
/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       CRC32.hpp - 32 bit Cyclic Redundancy Code
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2005-2012 Triple-IN GmbH Hamburg, Germany
 *
 * Copyright (c) 1998 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 * Last changes:
 * 21.05.2013 CBD:
 * - Changed: conform to T-I coding style
 * - Changed: get() will not reset the CRC
 * - New: add typedefs to get CRC example code for customers.
 */

#include "Global.hpp"

/**
 * Class CRC32 - Cyclic Redundancy Code,
 * A table-driven implementation of 32 bit CRC-checksums that
 * conforms to the CRC-32-IEEE 802.3 Ethernet Standard.
 * <p>&nbsp;<p>
 *
 * Summary:<br>
 * A CRC is a specific checksum algorithm that is designed to detect most
 * common data errors. The theory behind the CRC is described in a number
 * of publications.<p>
 * Implements the official polynomial used by CRC-32 in
 * PKZip, WinZip and Ethernet.
 * <p>&nbsp;<p>
 *
 * State Information:<br>
 * The constructor checks if the CRC lookup table is initialized and
 * generates a lookup table for each possible byte.
 * <p>&nbsp;<p>
 *
 *
 * References to External Specifications: <br>
 * "Programming embedded Systems" by Michael Barr, O'Reilly, 1st Edition 1999.
 *
 * @ingroup common
 */
class CRC32
{
    private:

        /**
         * The CRC parameters.  Currently configured for 32 bit CRC
         * Simply modify these to switch to another CRC standard.
         */
        enum
        {
            CRC32_POLYNOMIAL = 0x04c11db7, //
            CRC32_INITIAL_REMAINDER = 0xFFFFFFFF, //
            CRC32_FINAL_XOR_VALUE = 0xFFFFFFFF, //
            CRC32_WIDTH = 32, //
            CRC32_NULL = 0

        };

        /** number of bytes added */
        uint32_t mLength;

        /** this remainder is used in the add() method. It is reset by getCRC32() */
        uint32_t mRemainder;

        /**  a static flag indicating if the CRC table is initialized */
        static bool sCRC32IsInitialized;

        /**
         * An array containing the pre-computed intermediate result for each
         * possible byte of input.  This is used to speed up the computation.
         */
        static uint32_t sCRCTable[256];

        /**
         * Performs a reflection on a value:
         * swaps bit 7 with 0, 6 with 1 and so on.
         * Reflection is a requirement to conform to the
         * official CRC-32 standard.
         *
         * @param value
         *      the value to be reflected.
         * @param theBits
         *      number of bits to be swapped.
         * @return
         *      reflected value.
         */
        uint32_t
        reflect(uint32_t value, byte_t theBits);

    public:

        /**
         * The constructor.
         */
        CRC32();

        /**
         * Destructor.
         */
        ~CRC32();

        /**
         * Add some more data to compute a 32 bit CRC of several data.
         * <p>
         * Note: you should not use this function to add little endian integers
         * that are transmitted in big endian network byte order.<br>
         * Use addInt() instead.
         *
         * @param theDataPtr -
         *      a pointer on an array of data to add (null permitted).
         * @param theLength -
         *      number of bytes in the data array.
         *
         * @return current CRC.
         */
        uint32_t
        add(const void* theDataPtr, int32_t theLength);

        /**
         * Reset the checksum to initial stage.
         */
        inline void
        clear()
        {
            mLength = 0;
            mRemainder = CRC32_INITIAL_REMAINDER;
        }

        /**
         * Compute the 32 bit CRC for data which are previously added by add().
         *
         * @return the 32 bit CRC checksum.
         */
        inline uint32_t
        get()
        {
            return (mRemainder ^ CRC32_FINAL_XOR_VALUE);
        }

        /**
         * Compute a 32 bit CRC for the given data array.
         *
         * @param data - a pointer on an array of data.
         * @param len - number of bytes in the data array.
         *
         * @return the 32 bit CRC checksum.
         */
        inline uint32_t
        get(const void* data, int32_t len)
        {
            clear();
            return add(data, len);
        }

};

#endif /*CRC32_HPP_*/
