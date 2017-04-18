/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program

 * @file:       LinuxClientSocket.cpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         Linux 4
 *
 *
 * (c) 2008-2011 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 ****************************************************************************
 */

#if __linux__

#include "LinuxClientSocket.hpp"

extern "C"
{
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
    namespace unistd
    {
#include <unistd.h>
    }
#include <errno.h>
}

#include <cerrno>
#include <cstring>
#include <cstdio>

//#define DEBUG_WRITE 1
//#define DEBUG_READ 1

/*
 * Creates a socket and bind it to a server's address.
 */
ClientSocket::ClientSocket() :
mClientIPAddress(), //
mIsOpen(false),//
mLogFile(0),//
mServerIPAddress(),//
mSocketHandle(-1),//
mTimeout(0)
{
    // create the socket handle
    mSocketHandle = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 >= mSocketHandle)
    {
    	perror("socket failed");
        fprintf(stderr, "cannot create sockets.\r\n");
    }
}

/*
 * Destructor.
 */
ClientSocket::~ClientSocket()
{
    close();
}

/*
 * Closes the connection.
 */
ErrorID_t
ClientSocket::close()
{
    if (isOpen())
    {
        unistd::close(mSocketHandle);
        mIsOpen = false;
    }
    return ERR_SUCCESS;
}

/*
 * Setup the client's address.
 */
ErrorID_t
ClientSocket::setClientIPAddress(const char* theClientAddress, int32_t thePort)
{
    // setup the clients's socket address
    memset(&mClientIPAddress, 0, sizeof(mClientIPAddress));
    mClientIPAddress.sin_family = AF_INET;
    mClientIPAddress.sin_port = htons(thePort);
    mClientIPAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    return ERR_SUCCESS;
}

/*
 * Setup the server's address.
 */
ErrorID_t
ClientSocket::setServerIPAddress(const char* theServerAddress, int32_t thePort)
{
    // look up the remote host's IP address
    struct hostent* hostInfo = gethostbyname(theServerAddress);
    if (NULL == hostInfo)
    {
    	perror("gethostbyname failed");
        fprintf(stderr, "Socket error: unknown remote host.\r\n");
        return ERR_INVALID_HANDLE;
    }

    // setup the server's socket address
    mServerIPAddress.sin_family = AF_INET;
    memcpy((char *) &mServerIPAddress.sin_addr, hostInfo->h_addr, hostInfo->h_length);
    mServerIPAddress.sin_port = htons(thePort);
    return ERR_SUCCESS;
}

/*
 * Defines a timeout.
 */
ErrorID_t
ClientSocket::setTimeout(int32_t theTimeout)
{
    mTimeout = theTimeout;

    // apply to open connection
    if (isOpen())
    {
        close();
        return open();
    }

    return ERR_SUCCESS;
}

/*
 * Defines a log file to save received data.
 */
void
ClientSocket::setDataLogFile(FILE* theLogFile)
{
    mLogFile = theLogFile;
}

/*
 * Opens the connection.
 */
ErrorID_t
ClientSocket::open()
{
    int32_t status = 0;

    // typesafe cast.
    union
    {
        struct timeval asTimeVal;
        char asChar[sizeof(asTimeVal)];
    }lTimeout;
    lTimeout.asTimeVal.tv_sec = mTimeout;

    // bind port
    status = bind(mSocketHandle, (struct sockaddr*) &mClientIPAddress, sizeof(mClientIPAddress));
    if (0 > status)
    {
    	perror("bind failed");
        fprintf(stderr, "Socket error: Cannot bind connection/port\r\n");
        return ERR_INVALID_HANDLE;
    }

    // connect to the server
    status = connect(mSocketHandle, (struct sockaddr*) &mServerIPAddress, sizeof(mServerIPAddress));
    if (0 > status)
    {
    	perror("connect failed");
        fprintf(stderr, "Socket error: Connection refused.\r\n");
        return ERR_INVALID_HANDLE;
    }

    // set timeout
    status = setsockopt(mSocketHandle, SOL_SOCKET, SO_RCVTIMEO, lTimeout.asChar, sizeof(lTimeout));
    if (0 > status)
    {
    	perror("setsockopt failed");
        fprintf(stderr, "Socket error: Cannot initialize connection timeout! (%d)\r\n", status);
        return ERR_INVALID_HANDLE;
    }

    mIsOpen = true;
    return ERR_SUCCESS;
}

/*
 * POSIX similar read() method.
 * Reads up to len bytes of data from the input buffer device
 * into an array of bytes.
 */
int32_t
ClientSocket::read(void* buffer, int32_t size)
{
    int32_t result = 0;

    if (isOpen() && (0 != buffer) && (0 < size))
    {
        result = recv(mSocketHandle, static_cast<char*>(buffer), size, 0);

        // OK?
        if (0 < result)
        {
#if DEBUG_READ
        	printf("Read %d byte NET data!\r\n", result);
			for(int i = 0; i < result; i ++)
			{
				printf("0x%x ", *((unsigned char *)buffer+i));
			}
			printf("\r\n");
#endif
            // write received byte into a log file
            if (0 != mLogFile)
            {
                fwrite(buffer, result, 1, mLogFile);
            }
        }

        // handle most common errors
        else
        {
            // get socket error ID.
            result = -errno;
            //fprintf(stderr, "Socket error: cannot read (%d)\r\n", result);
        }
    }
    return result;
}

/*
 * POSIX similar write() method.
 * Writes len bytes from the specified byte array to the outgoing
 * buffer. <p>
 */
int32_t
ClientSocket::write(void* buffer, int32_t size)
{
    int32_t result = 0;

    if (isOpen() && (0 != buffer) && (0 < size))
    {
#if DEBUG_WRITE
    	printf("Writing NET data %d bytes!\r\n", size);
		for(int i = 0; i < size; i ++)
		{
			printf("0x%x ", *((unsigned char *)buffer+i));
		}
		printf("\r\n");
#endif
        result = send(mSocketHandle, static_cast<char*>(buffer), size, 0);
    }
    return result;
}

#endif /* __linux__ */
