/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       CommandBase.cpp
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
 *
 ****************************************************************************
 */

#include "CommandBase.hpp"

#ifdef  __WIN32__
#include "WinClientSocket.hpp"
#endif

#ifdef __linux__
#include "LinuxClientSocket.hpp"
#endif

/* Constructor */
CommandBase::CommandBase(IDataStream & theDataStream) :
        mBuffer(), //
        mBytesReceived(0), //
        mBufferPtr(), //
        mDataStream(theDataStream)
{
    mBufferPtr.asCharPtr = mBuffer;
}

/*
 * Standard destructor.
 */
CommandBase::~CommandBase()
{
    // nothing to be done.
}

/*
 * Calculates the CRC of a transmitter buffer and stores it
 * in the last 4 bytes of the buffer.
 */
ErrorID_t
CommandBase::calculateCRC(void* theBuffer, int32_t theBufferLength)
{
    if ((0 != theBuffer) && (4 < theBufferLength))
    {
        CRC32 lCRC;
        cast_ptr_t lBufferPtr = { theBuffer }; // casts the buffer to an array of integer
        const int32_t lCRCPosition = theBufferLength / sizeof(int32_t) - 1; // position of the CRC in the buffer

        const uint32_t lCRCValue = lCRC.get(theBuffer, theBufferLength - 4);
        lBufferPtr.asIntegerPtr[lCRCPosition] = htonl(lCRCValue);
        return ERR_SUCCESS;
    }
    return ERR_INVALID_PARAMETER;
}

/*
 *
 */
ErrorID_t
CommandBase::checkCRC()
{
    CRC32 lCRC;

    // typesafe cast. CRC in network byte order is expected in the last 4 bytes of the buffer
    cast_ptr_t lCRCReceivedPtr = { &mBuffer[mBytesReceived - 4] };
    const uint32_t lCRCExpected = htonl(*lCRCReceivedPtr.asIntegerPtr);
    const uint32_t lCRCReceived = lCRC.get(&mBuffer, mBytesReceived - 4);

    //printf("CRC expected:0x%x, received:0x%x\n", lCRCExpected, lCRCReceived);
    if (lCRCExpected != lCRCReceived)
    {
        return ERR_CRC;
    }
    return ERR_SUCCESS;
}

/* Handle error response */
ErrorID_t
CommandBase::checkErrors()
{
    const cast_ptr_t lData = { mBuffer };
    int32_t result = ERR_SUCCESS;

    // ID is ERR? Take error code
    if (ERR_COMMAND_ID == lData.asIntegerPtr[0])
    {
        result = htonl(lData.asIntegerPtr[2]);
    }
    return intToError(result);
}

/*
 * Converts the integer values of a buffer
 * from host byte order to network byte order,
 * except of the first 4 bytes. These first ASCII bytes contain
 * the command ID.
 */
void
CommandBase::convertHostToNetwork(void* theBuffer, int32_t theBufferLength)
{
    // skip command ID ASCII bytes
    const cast_ptr_t lData = { theBuffer };
    const int32_t lIntegerToConvert = (theBufferLength - COMMAND_ID_SIZE) / sizeof(int32_t);

    for (int32_t l = 1; l < lIntegerToConvert; l++)
    {
        lData.asIntegerPtr[l] = htonl(lData.asIntegerPtr[l]);
    }
}

/**
 * Converts the integer values of a buffer
 * from network byte order to host byte order,
 * except of the first 4 bytes. These first ASCII bytes contain
 * the command ID.
 */
void
CommandBase::convertNetworkToHost(void* theBuffer, int32_t theBufferLength)
{
    // skip command ID ASCII bytes
    cast_ptr_t lData = { theBuffer };
    const int32_t lIntegerToConvert = (theBufferLength - COMMAND_ID_SIZE) / sizeof(int32_t);

    for (int32_t l = 1; l < lIntegerToConvert; l++)
    {
        lData.asIntegerPtr[l] = ntohl(lData.asIntegerPtr[l]);
    }
}

/*
 * Transmits a buffer with a command and
 * receives the result.
 * Checks the data received for CRC errors and ERR responses.
 * Received data are stored in mBuffer.
 */
ErrorID_t
CommandBase::sendCommand(void* theCommandPtr, int32_t theCommandSize)
{
    ErrorID_t result = ERR_SUCCESS;

    try
    {
        // send command
        if (0 > mDataStream.write(theCommandPtr, theCommandSize))
        {
            throw ERR_WRITE;
        }

        // receive response
        mBytesReceived = mDataStream.read(mBuffer, sizeof(mBuffer));
        if (0 > mBytesReceived)
        {
            throw ERR_READ;
        }

        // check CRC
        if (ERR_SUCCESS != checkCRC())
        {
            throw ERR_CRC;
        }

        // parse for error messages
        result = checkErrors();
    }
    catch (ErrorID_t& e)
    {
        result = e;
    }
    return result;
}
