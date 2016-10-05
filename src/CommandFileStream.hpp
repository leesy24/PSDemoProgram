#ifndef CommandFileStream_HPP_
#define CommandFileStream_HPP_
/**
 ****************************************************************************
 *
 * Project:     Triple-IN KEM Sensor Firmware
 *
 * @file:       CommandFileStream.hpp - Read data from a file.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2014 Triple-IN GmbH Hamburg, Germany
 *
 * All rights reserved. Using, copying, publishing
 * or distributing  is not permitted without prior
 * written agreement of Triple-IN GmbH.
 ****************************************************************************
 * Last changes:
 */

#include <cstdio>
#include "IDataStream.hpp"

/**
 * Class CommandFileStream.hpp -
 * Read PS commands from a file.
 * <p>&nbsp;<p>
 *
 * <b>Executive summary:</b>
 * <p>
 * Uses the I/O functions to read data from a file.
 */
class CommandFileStream : public IDataStream
{
    private:

        /** the file handle, used to listen to data */
        FILE* mFileHandle;

        /** the name of the data file; provided by the parent class */
        const char* const mFileName;

        /** private assignment constructor to avoid misuse */
        explicit
        CommandFileStream(const CommandFileStream& src);

        /** private assignment operator to avoid misuse */
        CommandFileStream&
        operator =(const CommandFileStream& src);

    public:

        /**
         * Creates a file stream with the given file name.
         */
        CommandFileStream(const char* const theFileName);

        /**
         * Destructor.
         */
        virtual
        ~CommandFileStream();

        /**
         * Closes this Device and releases any system resources associated
         * with this object. <p>
         * The file handle is set to 0.
         *
         * @return -
         *      here always ERR_SUCCESS.
         */
        virtual ErrorID_t
        close();

        /**
         * Returns true if the file is ready to be used.
         */
        virtual bool
        isOpen();

        /**
         * Opens and resets the file.
         *
         * @return
         *      ERR_SUCCESS if the file could be opened successfully,
         *      negative error code if open() failed.
         */
        virtual ErrorID_t
        open();

        /**
         * Specific read method for Triple-IN commands.
         * Reads the command ID and the length from the file, checks the length against
         * the given buffer size and returns the result stored in the buffer - in
         * byte order as read.
         *
         * @param buffer-
         *      a buffer to read
         * @param size -
         *      the number of bytes to read
         * @return -
         *      the number bytes received or a negative error code if failed.
         */
        virtual int32_t
        read(void* buffer, int32_t size);

        /**
         * POSIX similar write() method. Here without meaning.
         *
         * @return always 0.
         */
        virtual int32_t
        write(void*, int32_t);
};

#endif /* CommandFileStream_HPP_ */
