/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GSC2Command.cpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2016/01/13 13:34:52 $
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

#include "GSC2Command.hpp"
#include <stdio.h>

//#define DEBUG_PARSE 1

/* Constructor */
GSC2Command::GSC2Command(IDataStream & theDataStream) :
        CommandBase(theDataStream), //
        mCommand() //
{
    // prepare the command
    memcpy(mCommand.mCommandID, "GSC2", 4);
    mCommand.mLength = sizeof(mCommand.mScanAvgNumber) + sizeof(mCommand.mPointAvgNumber);
}

/*
 * Standard destructor.
 */
GSC2Command::~GSC2Command()
{
    // nothing to be done.
}

/*
 * Clears a scan structure.
 */
void
GSC2Command::clearScan(Scan_t& theScan)
{
    theScan.mNumberOfParameter = 0;
    theScan.mNumberOfPoints = 0;

    for (int32_t l = 0; l < NUMBER_OF_SCAN_PARAMETER; l++)
    {
        theScan.mParameter[l] = 0;
    }
}

/*
 * Parses the receiver buffer and copy the result into the scan structure.
 */
ErrorID_t
GSC2Command::parseScan(Scan_t& theScan)
{
    // set moving integer pointer; skip command ID and length
    cast_ptr_t lDataPtr = { mBuffer };
    int32_t* lIntegerPtr = &lDataPtr.asIntegerPtr[2];

    // take length of parameter block
    int32_t lNumberOfParameter = *lIntegerPtr++;

    // check compatibility of firmware and control program
    if (lNumberOfParameter >= NUMBER_OF_SCAN_PARAMETER)
    {
        theScan.mNumberOfParameter = NUMBER_OF_SCAN_PARAMETER;
    }
    else
    {
        theScan.mNumberOfParameter = lNumberOfParameter;
    }

#if DEBUG_PARSE
    printf("Number of parameters = %d, %d\r\n", lNumberOfParameter, theScan.mNumberOfParameter);
#endif

    // copy known parameter to scan
    for (int32_t l = 0; l < theScan.mNumberOfParameter; l++)
    {
        theScan.mParameter[l] = *lIntegerPtr++;
#if DEBUG_PARSE
        printf("parameter[%d] = %d(0x%x)\r\n", l, theScan.mParameter[l], theScan.mParameter[l]);
#endif
    }

    // skip unkown parameter
    for (int32_t l = theScan.mNumberOfParameter; l < lNumberOfParameter; l++)
    {
#if DEBUG_PARSE
        printf("parameter[%d] = %d(0x%x)\r\n", l, *lIntegerPtr, *lIntegerPtr);
#endif
        lIntegerPtr++;
    }

    // get number of echoes. If 0, then the master echo is transfered instead of the number
    theScan.mNumberOfEchoes = theScan.mParameter[PARAMETER_NUMBER_OF_ECHOES];
    if (0 == theScan.mNumberOfEchoes)
    {
    	theScan.mNumberOfEchoes = 1;
    }

    // take number of points, check limits
    theScan.mNumberOfPoints = *lIntegerPtr++;
#if DEBUG_PARSE
    printf("Number of points = %d\r\n", theScan.mNumberOfPoints);
#endif
    if ((MAX_NUMBER_OF_ECHOS < theScan.mNumberOfEchoes) || (MAX_POINTS_PER_SCAN < theScan.mNumberOfPoints))
    {
        clearScan(theScan);
        return ERR_BUFFER_OVERFLOW;
    }

    // copy data block according to the data content.
    switch (theScan.mParameter[PARAMETER_DATA_CONTENT])
    {
        case NO_DATABLOCK:
            // no data available
            break;

        // copy distances only
        // loop through all points to copy distances and pulse width
        case DATABLOCK_WITH_DISTANCES:

            for (int32_t lPoints = 0; lPoints < theScan.mNumberOfPoints; lPoints++)
            {
                // loop for each point through all echos
                for (int32_t lEchos = 0; lEchos < theScan.mNumberOfEchoes; lEchos++)
                {
                    theScan.mScanData[lPoints][lEchos].mDistance = *lIntegerPtr++;
                } // end echos
            } // end points
            break;

            // default: distance and pulse width. If the echo number is included, we remove it.
        case DATABLOCK_WITH_DISTANCES_PW:
        case DATABLOCK_WITH_DISTANCES_PW_INCLUDES_ECHO:
        default:
            for (int32_t lPoints = 0; lPoints < theScan.mNumberOfPoints; lPoints++)
            {
                // loop for each point through all echos
                for (int32_t lEchos = 0; lEchos < theScan.mNumberOfEchoes; lEchos++)
                {
                    theScan.mScanData[lPoints][lEchos].mDistance = *lIntegerPtr++;
                    theScan.mScanData[lPoints][lEchos].mPulseWidth = *lIntegerPtr++;
                } // end echos
            } // end points
            break;

    } // end switch

    return ERR_SUCCESS;
}

/*
 * Performs the GSCN command.
 */
ErrorID_t
GSC2Command::performCommand(int32_t theScanAvgNumber, int32_t thePointAvgNumber, Scan_t &theScan, char* theDataLogFileName)
{
    ErrorID_t result = ERR_SUCCESS;

    // clear the scan
    clearScan(theScan);

    // prepare the command
    mCommand.mScanAvgNumber = theScanAvgNumber;
    mCommand.mPointAvgNumber = thePointAvgNumber;
    mCommand.mLength = sizeof(mCommand.mScanAvgNumber) + sizeof(mCommand.mPointAvgNumber);
    convertHostToNetwork(&mCommand, sizeof(mCommand));
    calculateCRC(&mCommand, sizeof(mCommand));

    // send command
    result = sendCommand(&mCommand, sizeof(mCommand));

    // convert to host byte order and copy
    if (ERR_SUCCESS == result)
    {
        FILE* lDataLogFile = 0;
        lDataLogFile = fopen(theDataLogFileName, "wb");
        if (lDataLogFile)
        {
        	fwrite(mBuffer, mBytesReceived, 1, lDataLogFile);
        	fclose(lDataLogFile);
        }
        convertNetworkToHost(mBuffer, mBytesReceived);
        result = parseScan(theScan);
    }
    return result;
}
