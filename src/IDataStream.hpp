#ifndef IDATASTREAM_HPP_
#define IDATASTREAM_HPP_

/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       IDataStream.hpp - Interface for data stream classes.
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
#include "Global.hpp"
#include "ErrorID.h"

/**
 * Class IDataStream.hpp - Interface for data stream classes.
 * <p>&nbsp;<p>
 *
 * <b>Executive summary:</b>
 * <p>
 * This interface defines the most common lowlevel I/O functions read() and
 * write() for data streams.
 *  <p>&nbsp;<p>
 *
 */
class IDataStream
{
    public:

        /** Mandatory C++ destructor. */
        inline virtual
        ~IDataStream()
        {
            // nothing more to do.
        }

        /**
         * Shall close the Device and releas any system resources associated.
         *
         * @return
         *      ERR_SUCCESS if the data stream was successfully closed,
         *      negative error code if failed.
         */
        virtual ErrorID_t
        close(void) = 0;

        /**
         * Returns true if the socket is ready to be used.
         */
        virtual bool
        isOpen() = 0;

        /**
         * Opens and resets a data stream.
         *
         * @return
         *      ERR_SUCCESS if the data stream could be opened successfully,
         *      negative error code if open() failed.
         */
        virtual ErrorID_t
        open() = 0;

        /**
         * POSIX similar read() method.
         * Reads up to len bytes of data from the input buffer device
         * into an array of bytes.
         * If no data are available because the end of the data stream has been
         * reached, the value -1 is returned.
         *
         * @param buffer-
         *      a buffer to read
         * @param size -
         *      the number of bytes to read
         * @return -
         *      the number bytes received or a negative error code if failed.
         */
        virtual int32_t
        read(void* buffer, int32_t size) = 0;

        /**
         * POSIX similar write() method.
         * Writes len bytes from the specified byte array to the outgoing
         * data stream. <p>
         *
         * @param buffer -
         *      the byte array to write (null permitted).
         * @param size -
         *      number of bytes to write.
         * @return -
         *      the number bytes written or a negative error code if failed.
         */
        virtual int32_t
        write(void* buffer, int32_t size) = 0;

};
#endif /* IDATASTREAM_HPP_ */
