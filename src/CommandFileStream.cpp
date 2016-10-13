/**
 ****************************************************************************
 *
 * Project:     Triple-IN KEM Sensor Firmware
 *
 * @file:       CommandFileStream.cpp
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
#include "CommandFileStream.hpp"

// needed for ntohl
#if  __WIN32__
	#include <winsock2.h>
#elif __linux__
	#include <arpa/inet.h>
#else
	#error "This program requires Linux or Win32."
#endif

/* Constructor. */
CommandFileStream::CommandFileStream(const char* const theFileName) :
        mFileHandle(0), mFileName(theFileName)
{
    // nothing more to do.

}

/* Destructor; closes the file if necessary. */
CommandFileStream::~CommandFileStream()
{
    if (isOpen())
    {
        fclose(mFileHandle);
    }
}

/*
 */
ErrorID_t
CommandFileStream::close()
{
    if (isOpen())
    {
        fclose(mFileHandle);
    }
    return ERR_SUCCESS;
}

/*
 * Returns true if the file is ready to be used.
 */
bool
CommandFileStream::isOpen()
{
    return (0 != mFileHandle);
}

/*
 * Opens and resets the file.
 */
ErrorID_t
CommandFileStream::open()
{
    mFileHandle = fopen(mFileName, "rb");
    if (0 != mFileHandle)
    {
        return ERR_SUCCESS;
    }
    return ERR_INVALID_PARAMETER;
}

/*
 * Specific read method for Triple-IN commands.
 */
int32_t
CommandFileStream::read(void* buffer, int32_t size)
{
    int32_t result = 0;
    char lCommandID[4] = { 0 };
    uint32_t lLength = 0;
    int32_t* lBufferPtr = static_cast<int32_t*>(buffer);
    int32_t lSize = 0;

    try
    {
        // read command ID
        if (4 > fread(lBufferPtr++, sizeof(lCommandID), 1, mFileHandle))
        {
            throw ERR_READ;
        }

        if (4 > fread(lBufferPtr++, sizeof(lLength), 1, mFileHandle))
        {
            throw ERR_READ;
        }

        // check buffer size
        lSize = ntohl(lBufferPtr[1]);
        if (size < lSize)
        {
            throw ERR_BUFFER_OVERFLOW;
        }

        // read data including CRC. The resulting number of bytes do not
        // include command ID and length.
        result = fread(lBufferPtr, lSize + 4, 1, mFileHandle) + 8;
    }
    catch(ErrorID_t& e)
    {
        result = e;
    }
    return result;
}

/*
 * POSIX similar write() method. Here without meaning.
 */
int32_t
CommandFileStream::write(void*, int32_t)
{
    return 0;
}

