/*
 * LinuxClientUART.cpp
 *
 *  Created on: 2016. 10. 22.
 *      Author: leesy
 */

#if __linux__

#include <arpa/inet.h>
#include <stdint.h>
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

//#define DEBUG_WRITE 1
//#define DEBUG_READ 1

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
	ssize_t n;
	int32_t total = 0;
	int state = 0;
	int32_t length = INT32_MAX;

#if DEBUG_READ
	printf("Reading UART data!\r\n");
#endif

	do
	{
		n = unistd::read(tty_fd, (unsigned char *)buffer + total, size - total);
		if (n < 0)
		{
			if (total == 0)
			{
#if DEBUG_READ
				printf("Read zero byte UART data!\r\n");
#endif
				return 0;
			}
			break;
		}

		total += n;
#if DEBUG_READ
		printf("Read total %d bytes!\r\n", total);
#endif

		if (state == 0) // Get length state.
		{
			if (total >= 8)
			{
				length = (int32_t)ntohl(*(uint32_t *)((unsigned char *)buffer + 4));
#if DEBUG_READ
				printf("Read format Length = %d\r\n", length);
#endif
				state = 1;
			}
		}
	} while(total < length + 12);

#if DEBUG_READ
	printf("Read %d byte UART data!\r\n", total);
#endif

	return total;
}

int32_t ClientUART::write(void* buffer, int32_t size)
{
	int32_t n;

#if DEBUG_WRITE
	printf("Writing UART data %d bytes!\r\n", size);
	for(int i = 0; i < size; i ++)
	{
		printf("0x%x ", *((unsigned char *)buffer+i));
	}
	printf("\r\n");
#endif
	n = unistd::write(tty_fd, buffer, size);

	//printf("Written UART data!\r\n");

	return n;
}

#endif // __linux__



