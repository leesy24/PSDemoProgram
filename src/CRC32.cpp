/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program

 * @file:       CRC32.cpp - Cyclic Redundancy Code
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2005 Triple-IN GmbH Hamburg, Germany
 *
 * Indicated part of codes:
 * Copyright (c) 1998 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 ****************************************************************************
 */
#include <stdio.h>
#include "CRC32.hpp"

/*  a local flag indicating if the CRC table was initialized */
bool CRC32::sCRC32IsInitialized = false;

/*
 * An array containing the pre-computed intermediate result for each
 * possible byte of input.  This is used to speed up the computation.
 */
uint32_t CRC32::sCRCTable[256];

/*
 * The constructor.
 * Initialize the CRC table.
 */
CRC32::CRC32() :
        // no base class constructor.
        mLength(0), //
        mRemainder(CRC32_INITIAL_REMAINDER)
{
    if (false == sCRC32IsInitialized)
    {
        // 256 values representing ASCII character codes.
        for (int iCodes = 0; iCodes <= 0xFF; iCodes++)
        {
            sCRCTable[iCodes] = reflect(iCodes, 8) << (CRC32_WIDTH - 8);

            for (int iPos = 0; iPos < 8; iPos++)
            {
                sCRCTable[iCodes] = //
                        (sCRCTable[iCodes] << 1) ^ (sCRCTable[iCodes] & (1 << 31) ? CRC32_POLYNOMIAL : CRC32_NULL);
            }

            sCRCTable[iCodes] = reflect(sCRCTable[iCodes], CRC32_WIDTH);
			//printf("0x%08x, ", sCRCTable[iCodes]);
        }
		//printf("\r\n");
        sCRC32IsInitialized = true;
    }
}

/*
 * Destructor.
 */
CRC32::~CRC32()
{
    // has nothing to do.
}

/*
 * Add some more data to compute a 32 bit CRC of several data.
 */
uint32_t
CRC32::add(const void* theDataPtr, int32_t theLength)
{
    // setup the pointer
    const unsigned char* lPtr = static_cast<const unsigned char*>(theDataPtr);

    // check parameter. Return default if there is nothing to do.
    if (0 == theDataPtr || 0 == theLength)
    {
        return 0;
    }

    // Divide the message by the polynomial, a byte at time.
    for (int32_t l = 0; l < theLength; l++)
    {
        mRemainder = (mRemainder >> 8) ^ sCRCTable[(mRemainder & 0xFF) ^ lPtr[l]];
    }

    // The final mRemainder is the CRC result.
    mLength += theLength;
    return (mRemainder ^ CRC32_FINAL_XOR_VALUE);
}

/*
 * Performs a reflection on a value.
 */
uint32_t
CRC32::reflect(uint32_t value, byte_t theBits)
{
    uint32_t result = 0;

    // Swap bit 0 for bit 7 bit 1 For bit 6, etc....
    for (int32_t bit = 1; bit < (theBits + 1); bit++)
    {
        if (value & 1)
        {
            result |= 1 << (theBits - bit);
        }
        value >>= 1;
    }
    return result;
}

