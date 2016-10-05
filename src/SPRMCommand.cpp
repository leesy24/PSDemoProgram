/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       SPRMCommand.cpp
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

#include "SPRMCommand.hpp"

/* Constructor */
SPRMCommand::SPRMCommand(IDataStream & theDataStream) :
        CommandBase(theDataStream), //
        mCommand() //
{
    // nothing to be done.
}

/*
 * Standard destructor.
 */
SPRMCommand::~SPRMCommand()
{
    // nothing to be done.
}

/*
 * Performs the SPRM command.
 */
ErrorID_t
SPRMCommand::performCommand(int32_t theParameterID, int32_t& theParameterValue)
{
    ErrorID_t result = ERR_SUCCESS;

    // prepare the command
    memcpy(mCommand.mCommandID, "SPRM", sizeof(mCommand.mCommandID));
    mCommand.mLength = sizeof(mCommand.mParameterID) + sizeof(mCommand.mParameterValue);
    mCommand.mParameterID = theParameterID;
    mCommand.mParameterValue = theParameterValue;
    convertHostToNetwork(&mCommand, sizeof(mCommand));
    calculateCRC(&mCommand, sizeof(mCommand));

    // clear result
    theParameterValue = 0;

    // send command
    result = sendCommand(&mCommand, sizeof(mCommand));
    if (ERR_SUCCESS == result)
    {
        // convert to host byte order
        convertNetworkToHost(mBuffer, mBytesReceived);

        // copy value actually set.
        theParameterValue = mBufferPtr.asIntegerPtr[SPRM_PARAMETER_VALUE_POSITION];
    }
    return result;
}
