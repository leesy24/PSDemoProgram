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

#define BUFFER_MAX (8*1024)

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

ErrorID_t ClientUART::config(const char* thePort, unsigned int theBaudRate, int32_t theTimeout, FILE* theLogFile)
{
	switch(theBaudRate)
	{
		case 50:
			mBaudRate = B50;
			break;
		case 75:
			mBaudRate = B75;
			break;
		case 110:
			mBaudRate = B110;
			break;
		case 134:
			mBaudRate = B134;
			break;
		case 150:
			mBaudRate = B150;
			break;
		case 200:
			mBaudRate = B200;
			break;
		case 300:
			mBaudRate = B300;
			break;
		case 600:
			mBaudRate = B600;
			break;
		case 1200:
			mBaudRate = B1200;
			break;
		case 1800:
			mBaudRate = B1800;
			break;
		case 2400:
			mBaudRate = B2400;
			break;
		case 4800:
			mBaudRate = B4800;
			break;
		case 9600:
			mBaudRate = B9600;
			break;
		case 19200:
			mBaudRate = B19200;
			break;
		case 38400:
			mBaudRate = B38400;
			break;
		case 57600:
			mBaudRate = B57600;
			break;
		case 115200:
			mBaudRate = B115200;
			break;
		case 230400:
			mBaudRate = B230400;
			break;
		case 460800:
			mBaudRate = B460800;
			break;
		default:
			return ERR_CONFIGURATION_ERROR;
	}

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
	cfsetospeed(&tio, (speed_t)mBaudRate);            // mBaudRate
	cfsetispeed(&tio, (speed_t)mBaudRate);            // mBaudRate
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

#define LOOP_MAX 100

int32_t ClientUART::read(void* buffer, int32_t size)
{
	ssize_t n;
	int32_t total = 0; // Init. total received data.
	int state = 0; // Init. state machine for getting length data of UDP data format.
	int32_t length = INT32_MAX - 12; // 12 = 4bytes Function code + 4bytes length + 4bytes CRC on UDP data format.
	int32_t loop = 0;

#if DEBUG_READ
	printf("Reading UART data!\r\n");
#endif

	do
	{
		n = unistd::read(tty_fd, (unsigned char *)buffer + total, size - total);
		if (n < 0)
		{
#if DEBUG_READ
			printf("Read error UART data!\r\n");
#endif
			return n;
		}
		else if (n == 0)
		{
			if (total == 0)
			{
#if DEBUG_READ
				printf("Read zero byte UART data! %d, %d\r\n", n, total);
#endif
				return 0;
			}
			// If read() return zero byte on timeout than break from loop.
			if (mTimeout != 0)
			{
#if DEBUG_READ
				printf("Read timeout! %d, %d\r\n", n, total);
#endif
				return 0;
			}
			else // read() return zero byte on non-blocking mode
			{
				loop ++;
				if (loop > LOOP_MAX)
				{
#if DEBUG_READ
					printf("Read zero byte UART data! %d, %d\r\n", n, total);
#endif
					return 0;
				}
			}
		}
		else
		{
			loop = 0;
		}

		total += n;
#if DEBUG_READ
		printf("Read total %d bytes!\r\n", total);
#endif

		if (state == 0) // If state machine is getting length data.
		{
			if (total >= 8) // If total received data is enough to get length data.
			{
				// Get length data from network endians data.
				length = (int32_t)ntohl(*(uint32_t *)((unsigned char *)buffer + 4));
#if DEBUG_READ
				printf("Read format Length = %d\r\n", length);
#endif
				if (length > BUFFER_MAX - 12)
				{
#if DEBUG_READ
					printf("Read format Length error!\r\n");
#endif
					return 0;
				}
				state = 1; // Set state machine to other.
			}
		}
	} while(total < length + 12); // Loop until total received data should reach as UDP data format(Function Code + Length + Data + CRC).

#if DEBUG_READ
	printf("Read %d byte UART data!\r\n", total);
//#if 0
	for(int32_t i = 0; i < total; i ++)
	{
		//printf("0x%x('%c') ", *((unsigned char *)buffer+i), *((unsigned char *)buffer+i));
		printf("0x%x ", *((unsigned char *)buffer+i));
	}
	printf("\r\n");
//#endif
#endif

	return (length + 12);
}

int32_t ClientUART::write(void* buffer, int32_t size)
{
	int32_t n;

#if DEBUG_WRITE
	printf("Writing UART data %d bytes!\r\n", size);
	for(int i = 0; i < size; i ++)
	{
		//printf("0x%x('%c') ", *((unsigned char *)buffer+i), *((unsigned char *)buffer+i));
		printf("0x%x ", *((unsigned char *)buffer+i));
	}
	printf("\r\n");
#endif
	n = unistd::write(tty_fd, buffer, size);

	//printf("Written UART data!\r\n");

	return n;
}

#endif // __linux__



