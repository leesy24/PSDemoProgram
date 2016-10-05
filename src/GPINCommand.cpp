/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GPINCommand.cpp
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

#include "GPINCommand.hpp"

/* Constructor */
GPINCommand::GPINCommand(IDataStream & theDataStream) :
        CommandBase(theDataStream), //
        mCommand(), //
        mParameterID(0), //
        mParameterInformation(), //
        mMaximumValue(0), //
        mMinimumValue(0), //
        mValue(0) //
{
    // nothing to be done.
}

/*
 * Standard destructor.
 */
GPINCommand::~GPINCommand()
{
    // nothing to be done.
}

/**
 * Clears the parameter.
 */
void
GPINCommand::clearParameterInfo()
{
    mParameterID = 0;
    mParameterInformation[0] = 0;
    mMaximumValue = 0;
    mMinimumValue = 0;
    mValue = 0;

}
/*
 * Performs the GPIN command.
 */
ErrorID_t
GPINCommand::performCommand(int32_t theParameterID)
{
    ErrorID_t result = ERR_SUCCESS;
    CRC32 lcrc;

    // prepare command structure
    memcpy(mCommand.mCommandID, "GPIN", 4);
    mCommand.mLength = 4;
    mCommand.mParameterID = theParameterID;
    convertHostToNetwork(&mCommand, sizeof(mCommand));
    calculateCRC(&mCommand, sizeof(mCommand));
    clearParameterInfo();

    // send command
    result = sendCommand(&mCommand, sizeof(mCommand));

    // get information
    if (ERR_SUCCESS == result)
    {
        // convert min, max and value; keep info string
        convertNetworkToHost(mBuffer, GPIN_INFO_OFFSET);

        // check resulting parameter ID
        cast_ptr_t lData = { mBuffer };
        if (0 != lData.asIntegerPtr[2])
        {
            // copy results
            mValue = lData.asIntegerPtr[4];
            mMinimumValue = lData.asIntegerPtr[5];
            mMaximumValue = lData.asIntegerPtr[6];

            // copy information.
            const char* ptr = &mBuffer[GPIN_INFO_OFFSET];
            strncpy(mParameterInformation, ptr, sizeof(mParameterInformation));
        }
    }
    return result;
}
