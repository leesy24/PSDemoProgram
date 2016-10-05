/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       CommandBase.hpp  - Base class for PS Laser Scanner commands.
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
 * Last changes:
 * 24.04.2014 CBD:
 * - Changed: use IDataStream for communication.
 * - New: support Linux sockets,
 */

#ifndef CommandBase_HPP_
#define CommandBase_HPP_

extern "C"
{
#include "ErrorID.h"
}
#include "Global.hpp"
#include "CRC32.hpp"
#include "IDataStream.hpp"
#include <cstring>

/**
 * Class CommandBase.hpp - Base class for PS Laser Scanner commands.
 */
class CommandBase
{
    protected:

        /** class constants */
        enum
        {
            /** maximum command/response size */
            MAX_COMMAND_SIZE = 64 * 1024,

            /** length of the command ID */
            COMMAND_ID_SIZE = 4,

            /** "ERR\0" in integer format */
            ERR_COMMAND_ID = 0x00525245,

            /** SYNC in integer format */
            SYNC_COMMAND_ID = 0x434e5953
        };

        /** buffer to store received data */
        char mBuffer[MAX_COMMAND_SIZE];

        /** number of bytes in the buffer */
        int32_t mBytesReceived;

        /** casting pointer on the buffer */
        cast_ptr_t mBufferPtr;

        /**
         * the data stream to read and write bytes.
         */
        IDataStream& mDataStream;

        /**
         * Calculates the CRC of a transmitter buffer and stores it
         * in the last 4 bytes of the buffer.
         * The CRC is stored in network byte order. After this, the buffer
         * is ready to send on the transmitter channel.
         *
         * @param theBuffer
         *      Pointer at the transmitter buffer.
         * @param theBufferLength
         *      bytes to be transmitted; including the CRC to be added.
         * @return
         *      ERR_SUCCESS if OK, otherwise a negative error code.
         */
        static ErrorID_t
        calculateCRC(void* theBuffer, int32_t theBufferLength);

        /**
         * Calculates the CRC of a received buffer and compares it with the
         * CRC appended to the buffer.
         * The CRC (in network byte order) is expected in the last 4 bytes.
         *
         * @return
         *      ERR_SUCCESS if OK, otherwise a negative error code.
         * @retval ERR_CRC
         *      in case the CRC does not match and is not 0.
         */
        virtual ErrorID_t
        checkCRC();

        /**
         * Checks a buffer for ERR responses.
         *
         * @return
         *      ERR_SUCCESS if OK, otherwise the negative error code from
         *      the result.
         */
        virtual ErrorID_t
        checkErrors();

        /**
         * Transmits a buffer with a command and
         * receives the result.
         * Checks the data received for CRC errors and ERR responses.
         * Received data are stored in mBuffer.
         *
         * @param theCommandPtr
         *      Pointer at a buffer with the command data to be send.
         * @param theCommandSize
         *      Size of the command data buffer.
         *
         * @return
         *      ERR_SUCCESS if OK, otherwise a negative error code.
         */
        virtual ErrorID_t
        sendCommand(void* theCommandPtr, int32_t theCommandSize);

        /* private assignment constructor to avoid misuse */
        explicit
        CommandBase(const CommandBase&);

        /* private assignment operator to avoid misuse */
        CommandBase&
        operator =(const CommandBase&);

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the Data Stream to be used for lowlevel I/O.
         */
        CommandBase(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~CommandBase();

        /**
         * Converts the integer values of a transmitter buffer
         * from host byte order to network byte order,
         * except of the first 4 bytes. These first ASCII bytes contain
         * the command ID.
         *
         * @param theBuffer
         *      Pointer at the transmitter buffer.
         * @param theBufferLength
         *      bytes to be transmitted; including the CRC to be added.
         */
        static void
        convertHostToNetwork(void* theBuffer, int32_t theBufferLength);

        /**
         * Converts the integer values of a receiver buffer
         * from network byte order to host byte order,
         * except of the first 4 bytes. These first ASCII bytes contain
         * the command ID.
         *
         * @param theBuffer
         *      Pointer at the receiver buffer.
         * @param theBufferLength
         *      bytes received; including the CRC.
         */
        static void
        convertNetworkToHost(void* theBuffer, int32_t theBufferLength);
};

#endif /* CommandBase_HPP_ */
