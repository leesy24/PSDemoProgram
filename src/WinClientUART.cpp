/*
 * ClientCommunication.cpp
 *
 *  Created on: 2016. 10. 21.
 *      Author: leesy
 */

#if __WIN32__

#include "WinClientUART.hpp"

ClientUART::ClientUART() :
        mIsOpen(false), //
		hComm(0), //
        mLogFile(0), //
        mTimeout(0)
{
}

ClientUART::~ClientUART()
{
	close();
}

ErrorID_t ClientUART::config(int32_t theTimeout, FILE* theLogFile)
{
	// set in ms
    mTimeout = theTimeout * 1000;

    mLogFile = theLogFile;

    return ERR_SUCCESS;
}

ErrorID_t ClientUART::open(const char* thePort)
{
	BOOL  Status;                          // Status of the various operations

	printf("Opening UART port %s~\r\n", thePort);

	if (thePort == NULL)
	{
        fprintf(stderr, "UART error: port not defined.\r\n");
        return ERR_INVALID_HANDLE;
	}

	/*---------------------------------- Opening the Serial Port -------------------------------------------*/
	hComm = CreateFile( thePort,        		      // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
						0,                            // No Sharing, ports cant be shared
						NULL,                         // No Security
					    OPEN_EXISTING,                // Open existing port only
						FILE_ATTRIBUTE_NORMAL,        // Non Overlapped I/O
                        NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "    Error! - Port %s can't be opened\r\n", thePort);
        return ERR_INVALID_HANDLE;
	}
	printf("    Port %s Opened\r\n", thePort);

	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/
	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE)
	{
		fprintf(stderr, "    Error! in GetCommState()\r\n");
        return ERR_INVALID_HANDLE;
	}

	dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate = 115200
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

	if (Status == FALSE)
	{
		fprintf(stderr, "    Error! in Setting DCB Structure\r\n");
        return ERR_INVALID_HANDLE;
	}
	printf("    Setting DCB Structure Successfull\r\n\r\n");
	printf("       Baudrate = %ld\r\n", dcbSerialParams.BaudRate);
	printf("       ByteSize = %d\r\n", dcbSerialParams.ByteSize);
	printf("       StopBits = %d\r\n", dcbSerialParams.StopBits);
	printf("       Parity   = %d\r\n", dcbSerialParams.Parity);

	/*------------------------------------ Setting Timeouts --------------------------------------------------*/
	COMMTIMEOUTS timeouts = { 0 };
#if 1 // Ref. https://groups.google.com/forum/#!topic/comp.os.ms-windows.programmer.win32/SotVc2_Eiig
	timeouts.ReadIntervalTimeout         = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant    = 0;
	timeouts.ReadTotalTimeoutMultiplier  = 0;
	timeouts.WriteTotalTimeoutConstant   = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
#else
	timeouts.ReadIntervalTimeout         = 50;
	timeouts.ReadTotalTimeoutConstant    = 50;
	timeouts.ReadTotalTimeoutMultiplier  = 10;
	timeouts.WriteTotalTimeoutConstant   = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
#endif

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
        return ERR_SUCCESS;
    }

	CloseHandle(hComm);//Closing the Serial Port

	mIsOpen = false;

	printf("Close UART success!\r\n");

	return ERR_SUCCESS;
}

int32_t ClientUART::read(void* buffer, int32_t size)
{
	BOOL  Status;			// Status of the various operations
	DWORD NoBytesRead = 0;	// Bytes read by ReadFile()

	//printf("Reading UART data!\r\n");
	if (!mIsOpen)
	{
		//printf("Read error : UART not opened!\r\n");
		return 0;
	}

	Status = ReadFile(hComm, buffer, size, &NoBytesRead, NULL);
	if (Status == FALSE || NoBytesRead == 0)
	{
		//printf("Read zero byte UART data!\r\n");
		return 0;
	}

	//printf("Read %ld byte UART data!\r\n", NoBytesRead);
	return (int32_t)NoBytesRead;
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
