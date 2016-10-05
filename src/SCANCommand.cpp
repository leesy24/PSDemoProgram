/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       SCANCommand.cpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/11/30 08:24:53 $
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

#include "SCANCommand.hpp"

/* Constructor */
SCANCommand::SCANCommand(IDataStream & theDataStream) :
        CommandBase(theDataStream), //
        mCommand() //
{
    // fill command structure
    CRC32 lcrc;
    memcpy(mCommand.mCommandID, "SCAN", 4);
    mCommand.mLength = 4;
    mCommand.mParameter = 1;
    convertHostToNetwork(&mCommand, sizeof(mCommand));
    calculateCRC(&mCommand, sizeof(mCommand));
}

/*
 * Standard destructor.
 */
SCANCommand::~SCANCommand()
{
    // nothing to be done.
}

/*
 * Performs the SCAN command.
 */
ErrorID_t
SCANCommand::performCommand()
{
    // the command was prepared by the constructor
    return sendCommand(&mCommand, sizeof(mCommand));
}

/**
 * Set a parameter for SCAN.
 *
 * @param theParameter
 *      the parameter value to be set.
 */
void
SCANCommand::setParameter(int32_t theParameter)
{
    mCommand.mParameter = theParameter;
    mCommand.mLength = 4;
    convertHostToNetwork(&mCommand, sizeof(mCommand));
    calculateCRC(&mCommand, sizeof(mCommand));
}

