/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GVERCommand.cpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/11/30 08:23:47 $
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

#include "GVERCommand.hpp"

/* Constructor */
GVERCommand::GVERCommand(IDataStream & theDataStream) :
        CommandBase(theDataStream), //
        mCommand() //
{
    CRC32 lcrc;

    // fill command structure
    memcpy(mCommand.mCommandID, "GVER", 4);
    mCommand.mLength = 0;
    calculateCRC(&mCommand, 12);
}

/*
 * Standard destructor.
 */
GVERCommand::~GVERCommand()
{
    // nothing to be done.
}

/*
 * Performs the GVER command.
 */
ErrorID_t
GVERCommand::performCommand()
{
    ErrorID_t result = ERR_SUCCESS;

    // the command was prepared by the constructor
    result = sendCommand(&mCommand, sizeof(mCommand));

    // copy result
    if (ERR_SUCCESS == result)
    {
        strncpy(mVersion, &mBuffer[8], sizeof(mVersion));
    }
    else
    {
        // clear version
        mVersion[0] = 0;
    }
    return result;
}
