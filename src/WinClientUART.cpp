/*
 * WinClientUART.cpp
 *
 *  Created on: 2016. 10. 21.
 *      Author: leesy
 */

#if defined(__WIN32__) || (defined(__CYGWIN__) && !defined(_WIN32))

#include "WinClientUART.hpp"
// needed for ntohl
#include <winsock2.h>

//#define DEBUG_WRITE 1
//#define DEBUG_READ 1

#define BUFFER_MAX (8*1024)

ClientUART::ClientUART() :
        mIsOpen(false), //
		mPort("\0"), //
		hComm(0), //
        mLogFile(0), //
        mTimeout(0)
{
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
		case CBR_110:
		case CBR_300:
		case CBR_600:
		case CBR_1200:
		case CBR_2400:
		case CBR_4800:
		case CBR_9600:
		case CBR_14400:
		case CBR_19200:
		case CBR_38400:
		case CBR_56000:
		case CBR_57600:
		case CBR_115200:
		case CBR_128000:
		case CBR_256000:
			break;
		default:
			return ERR_CONFIGURATION_ERROR;
	}

	strcpy(mPort, thePort);
	mBaudRate = theBaudRate;

	// set in ms
    mTimeout = theTimeout * 1000;

    mLogFile = theLogFile;

    return ERR_SUCCESS;
}

ErrorID_t ClientUART::open()
{
	BOOL  Status;                          // Status of the various operations

	printf("Opening UART port %s~\r\n", mPort);

	if (mPort == NULL)
	{
        fprintf(stderr, "UART error: port not defined.\r\n");
        return ERR_INVALID_HANDLE;
	}

	/*---------------------------------- Opening the Serial Port -------------------------------------------*/
	hComm = CreateFile( mPort,        		      // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
						0,                            // No Sharing, ports cant be shared
						NULL,                         // No Security
					    OPEN_EXISTING,                // Open existing port only
						FILE_ATTRIBUTE_NORMAL,        // Non Overlapped I/O
                        NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "    Error! - Port %s can't be opened\r\n", mPort);
        return ERR_INVALID_HANDLE;
	}
	printf("    Port %s Opened\r\n", mPort);

	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/
	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE)
	{
		fprintf(stderr, "    Error! in GetCommState()\r\n");
        return ERR_INVALID_HANDLE;
	}

	SetupComm (hComm, BUFFER_MAX, BUFFER_MAX);  /* Set buffer size. */
	PurgeComm (hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	dcbSerialParams.BaudRate = mBaudRate;      // Setting BaudRate = mBaudRate
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None
	dcbSerialParams.fBinary = TRUE;
	dcbSerialParams.fParity = FALSE;
	dcbSerialParams.fOutxCtsFlow = FALSE;
	dcbSerialParams.fOutxDsrFlow = FALSE;
	dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
	dcbSerialParams.fDsrSensitivity = FALSE;
	dcbSerialParams.fTXContinueOnXoff = FALSE;
	dcbSerialParams.fOutX = FALSE;
	dcbSerialParams.fInX = FALSE;
	dcbSerialParams.fErrorChar = FALSE;
	dcbSerialParams.fNull = FALSE;
	dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
	dcbSerialParams.fAbortOnError = FALSE;

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

	if (Status == FALSE)
	{
		fprintf(stderr, "    Error! in Setting DCB Structure\r\n");
        return ERR_INVALID_HANDLE;
	}
	printf("    Setting DCB Structure Successfull\r\n\r\n");
	printf("       Baudrate = %ld\r\n", dcbSerialParams.BaudRate);
	printf("       ByteSize = %d\r\n", dcbSerialParams.ByteSize);
	printf("       StopBits = %s\r\n", dcbSerialParams.StopBits == 0?"1bit":"1.5or2bits");
	printf("       Parity   = %s(%d)\r\n", dcbSerialParams.Parity == 0?"No":"Even/ODD", dcbSerialParams.Parity);

	/*------------------------------------ Setting Timeouts --------------------------------------------------*/
	COMMTIMEOUTS timeouts = { 0 };
	if (mTimeout == 0)
	{
		// Ref. https://groups.google.com/forum/#!topic/comp.os.ms-windows.programmer.win32/SotVc2_Eiig
		timeouts.ReadIntervalTimeout         = MAXDWORD;
		timeouts.ReadTotalTimeoutMultiplier  = 0;
		timeouts.ReadTotalTimeoutConstant    = 0;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant   = 0;
	}
	else
	{
		// Ref. https://msdn.microsoft.com/en-us/library/windows/desktop/aa363190(v=vs.85).aspx
		timeouts.ReadIntervalTimeout         = MAXDWORD;
		timeouts.ReadTotalTimeoutMultiplier  = MAXDWORD;
		timeouts.ReadTotalTimeoutConstant    = mTimeout;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant   = 0;
	}

	if (SetCommTimeouts(hComm, &timeouts) == FALSE)
	{
		fprintf(stderr, "    Error! in Setting Time Outs\r\n");
        return ERR_INVALID_HANDLE;
	}
	printf("    Setting Serial Port Timeouts Successfull\r\n");

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

	CloseHandle(hComm);//Closing the Serial Port

	mIsOpen = false;

	printf("Close UART success!\r\n");

	return ERR_SUCCESS;
}

#define LOOP_MAX 100

int32_t ClientUART::read(void* buffer, int32_t size)
{
	BOOL  Status;			// Status of the various operations
	DWORD NoBytesRead = 0;	// Bytes read by ReadFile()
	DWORD dSize = (DWORD)size;
	int32_t total = 0; // Init. total received data.
	int state = 0; // Init. state machine for getting length data of UDP data format.
	int32_t length = INT32_MAX - 12; // 12 = 4bytes Function code + 4bytes length + 4bytes CRC on UDP data format.
	int32_t loop = 0;

#if DEBUG_READ
	printf("Reading UART data!\r\n");
#endif
	if (!mIsOpen)
	{
#if DEBUG_READ
		printf("Read error : UART not opened!\r\n");
#endif
		return -1;
	}

	do
	{
		Status = ReadFile(hComm, (unsigned char *)buffer + total, dSize - total, &NoBytesRead, NULL);
		if (Status == FALSE)
		{
#if DEBUG_READ
			printf("Read error UART data!\r\n");
#endif
			return -1;
		}

		if (NoBytesRead == 0)
		{
			if (total == 0)
			{
#if DEBUG_READ
				printf("Read zero byte UART data! %d, %d\r\n", NoBytesRead, total);
#endif
				return 0;
			}
			// If read() return zero byte on timeout than break from loop.
			if (mTimeout != 0)
			{
#if DEBUG_READ
				printf("Read timeout! %d, %d\r\n", NoBytesRead, total);
#endif
				return 0;
			}
			else // read() return zero byte on non-blocking mode
			{
				loop ++;
				if (loop > LOOP_MAX)
				{
#if DEBUG_READ
					printf("Read zero byte UART data! %d, %d\r\n", NoBytesRead, total);
#endif
					return 0;
				}
			}
		}
		else
		{
			loop = 0;
		}

		total += (int32_t)NoBytesRead;
#if DEBUG_READ
		printf("Read total %d bytes!\r\n", total);
#endif

		if (state == 0) // If state machine is getting length data.
		{
			if (total >= 8) // If total received data is enough to get length data.
			{
				// Get length data from network endians data.
				length = ntohl(*(unsigned int *)((unsigned char *)buffer + 4));
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
		printf("0x%x('%c') ", *((unsigned char *)buffer+i), *((unsigned char *)buffer+i));
		//printf("0x%x ", *((unsigned char *)buffer+i));
	}
	printf("\r\n");
//#endif
#endif

	return (length + 12);
}

int32_t ClientUART::write(void* buffer, int32_t size)
{
	BOOL  Status;					// Status of the various operations
	DWORD dNoOfBytesWritten = 0;	// No of bytes written to the port

	//printf("Writing UART data %d bytes!\r\n", n);

	if (!mIsOpen)
	{
		return -1;
	}

#if DEBUG_WRITE
	printf("Writing UART data %d bytes!\r\n", size);
	for(int i = 0; i < size; i ++)
	{
		//printf("0x%x('%c') ", *((unsigned char *)buffer+i), *((unsigned char *)buffer+i));
		printf("0x%x ", *((unsigned char *)buffer+i));
	}
	printf("\r\n");
#endif

	Status = WriteFile(	hComm,				// Handle to the Serialport
						buffer,				// Data to be written to the port
						size,				// No of bytes to write into the port
						&dNoOfBytesWritten,	// No of bytes written to the port
						NULL);

	if (Status == FALSE)
		return -1;

	//printf("Written UART data!\r\n");
	return (int32_t)dNoOfBytesWritten;
}

#endif // __WIN32__
