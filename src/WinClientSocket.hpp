#ifndef ClientSocket_HPP_
#define ClientSocket_HPP_

/**
 ****************************************************************************
 *
 * Project:     Triple-IN Sensor Simulation
 *
 * @file:       ClientSocket.hpp -
 *              Client Socket to connect to a Triple-IN Sensor.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         MinGW 4
 *
 *
 * (c) 2013     Triple-IN GmbH Hamburg, Germany
 *
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 * Last changes:
 * 21.11.2013 CBD:
 * - New: handle socket errors.
 */

// Windows version
#ifdef  __WIN32__
extern "C"
{
#include <winsock2.h>
}
#endif

// Linux version
#if __linux__

// disable windows calls
#define __WIN32__ (0)

extern "C"
{
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
    namespace unistd
    {
    #include <unistd.h>
    }
}
#endif

#include <cstdio>
#include "IDataStream.hpp"

/**
 * Class ClientSocket.hpp -
 * Client Socket to connect to a Triple-IN Sensor.
 * <p>&nbsp;<p>
 *
 * <b>Executive summary:</b>
 * <p>
 * Uses the standard socket functions to establish a client/server
 * connection to a Triple-IN sensor.
 * <p>
 * When programming with sockets, usually a server and some client programs
 * are implemented. The server will wait and listening for incoming
 * connections from clients and handle them.
 * <p>
 * Clients can connect to servers by use of the server internet address and
 * the port number.
 * <p>
 * <p>&nbsp;<p>
 *
 *
 * <b>OS/Hardware Dependencies</b>
 * <p>
 * This module uses the base functions of the Berkley Socket library.
 * Only the constructor and the destructor make use of Microsoft
 * Windows-specific extensions in order to connect and disconnect to
 * the Windows socket DLL.
 * <p>
 * Tested on Windows 7 with the MinGW compiler.
 * <p>&nbsp;<p>
 *
 *
 * <b>References to External Specifications:</b>
 * <p>
 *  /1/ "Windows Sockets. An Open Interface for Network Programming under
 *  Microsoft Windows. Version 1.1, 20 January 1993",<br>
 *  <ref http://www.sockets.com/winsock.htm>
 *
 */
class ClientSocket : public IDataStream
{
private:
        /** the clients (our) IP address */
        sockaddr_in mClientIPAddress;

        /** a flag indicating an opened socket. */
        bool mIsOpen;

        /** Pointer at a log file to store received data. */
        FILE *mLogFile;

        /** the sensors IP address */
        struct sockaddr_in mServerIPAddress;

        /** the socket handle, used to listen to data */
        int mSocketHandle;

        /** timeout in ms. */
        int mTimeout;

        /** private assignment constructor to avoid misuse */
        explicit
        ClientSocket(const ClientSocket& src);

        /** private assignment operator to avoid misuse */
        ClientSocket&
        operator =(const ClientSocket& src);

    public:

        /**
         * Creates a socket.
         */
        ClientSocket();

        /**
         * Destructor.
         */
        virtual
        ~ClientSocket();

        /**
         * Closes this Device and releases any system resources associated
         * with this object. <p>
         * The socket handle is set to 0.
         *
         * @return -
         *      here always ERR_SUCCESS.
         */
        virtual ErrorID_t
        close();

        /**
         * Returns true if the socket is ready to be used.
         */
        inline bool
        isOpen()
        {
            return mIsOpen;
        }

        /**
         * Opens and resets a hardware device.
         *
         * @return
         *      ERR_SUCCESS if the device could be opened successfully,
         *      negative error code if open() failed.
         */
        virtual ErrorID_t
        open();

        /**
         * POSIX similar read() method.
         * Reads up to len bytes of data from the input buffer device
         * into an array of bytes.
         * If no data are available because the end of the datagram has been
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
        read(void* buffer, int32_t size);

        /**
         * Setup the client's address.
         * @param theClientAddress
         *      String with the client IP address (e.g. "127.0.0.1")
         * @param thePort
         *      the Port number
         */
        ErrorID_t
        setClientIPAddress(const char* theClientAddress, int32_t thePort);

        /**
         * Setup the server's address.
         *
         * @param theServerAddress
         *      String with the IP address (e.g. "10.0.1.200")
         * @param thePort
         *      the Port number
         */
        ErrorID_t
        setServerIPAddress(const char* theServerAddress, int32_t thePort);

        /**
         * Defines a log file to save received data.
         * Set to 0 to disable logging.
         *
         * @param theLogFile
         *      Pointer at a file handle; set to 0 to disable logging.
         */
        void
        setDataLogFile(FILE* theLogFile);

        /**
         * Defines a timeout.
         * Set to 0 to disable.
         *
         * @param theTimeout
         *      Socket timeout in seconds.
         * @return
         *      ERR_SUCCESS if OK, otherwise a negative error code.
         */
        ErrorID_t
        setTimeout(int32_t theTimeout);

        /**
         * POSIX similar write() method.
         * Writes len bytes from the specified byte array to the outgoing
         * buffer. <p>
         *
         * @param buffer -
         *      the byte array to write (null permitted).
         * @param size -
         *      number of bytes to write.
         * @return -
         *      the number bytes written or a negative error code if failed.
         */
        virtual int32_t
        write(void* buffer, int32_t size);
};
#endif /* ClientSocket_HPP_ */
