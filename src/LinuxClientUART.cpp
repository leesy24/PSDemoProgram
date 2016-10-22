/*
 * LinuxClientUART.cpp
 *
 *  Created on: 2016. 10. 22.
 *      Author: leesy
 */

#if __linux__

#include <sys/types.h>
#include <sys/stat.h>
namespace unistd {
	#include <unistd.h>
}
namespace fcntl {
	#include <fcntl.h>
}
#include <string.h>
#include "LinuxClientUART.hpp"

ClientUART::ClientUART() :
        mIsOpen(false), //
		tty_fd(0), //
        mLogFile(0), //
        mTimeout(0)
{
	strcpy(mPort, "\0");
}

ClientUART::~ClientUART()
{
	if (mIsOpen)
		close();
}

ErrorID_t ClientUART::config(const char* thePort, int32_t theTimeout, FILE* theLogFile)
{
	strcpy(mPort, thePort);

	// set in ms
    mTimeout = theTimeout * 1000;

    mLogFile = theLogFile;

    return ERR_SUCCESS;
}

ErrorID_t ClientUART::open()
{
	printf("Opening UART port %s~\r\n", mPort);

	if (mPort == NULL)
	{
        fprintf(stderr, "UART error: port not defined.\r\n");
        return ERR_INVALID_HANDLE;
	}

	tty_fd = fcntl::open(mPort, O_RDWR/* | O_NONBLOCK*/);
	if (tty_fd == 0)
	{
		fprintf(stderr, "Error: open with %s.\r\n", mPort);
        return ERR_INVALID_HANDLE;
	}
	printf("Open UART port %s success!\r\n", mPort);

	tcgetattr(tty_fd, &old_tio);
	memset(&tio,0,sizeof(tio));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag = 0;
	if (mTimeout == 0)
	{
		tio.c_cc[VMIN] = 0;
		tio.c_cc[VTIME] = 0;
	}
	else
	{
		tio.c_cc[VMIN] = 0;
		tio.c_cc[VTIME] = mTimeout / 100;
	}
	cfsetospeed(&tio, B115200);            // 115200 baud
	cfsetispeed(&tio, B115200);            // 115200 baud
	tcsetattr(tty_fd, TCSANOW, &tio);

	mIsOpen = true;
    return ERR_SUCCESS;
}

ErrorID_t ClientUART::close()
{
	printf("Closing UART~\r\n");
    if (!mIsOpen)
    {
    	printf("Close is not opened!\r\n");
        return ERR_SUCCESS;
    }

    tcsetattr(tty_fd, TCSANOW, &old_tio);
    unistd::close(tty_fd);

	mIsOpen = false;

	printf("Close UART success!\r\n");

	return ERR_SUCCESS;
}

int32_t ClientUART::read(void* buffer, int32_t size)
{
	int32_t n;
	printf("Reading UART data!\r\n");

	n = unistd::read(tty_fd, buffer, size);

	printf("Read %d byte UART data!\r\n", n);
	return n;
}

int32_t ClientUART::write(void* buffer, int32_t size)
{
	int32_t n;

	//printf("Writing UART data %d bytes!\r\n", size);

	n = unistd::write(tty_fd, buffer, size);

	//printf("Written UART data!\r\n");

	return n;
}

#endif // __linux__



