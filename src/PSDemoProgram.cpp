/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       PSDemoProgram.cpp -
 *              Little give-away software for PS laser sensors.
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
 *
 ****************************************************************************
 */

#include "PSDemoProgram.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if __linux__
#include "LinuxClientSocket.hpp"
#include "LinuxClientUART.hpp"
#elif __WIN32__
#include "WinClientSocket.hpp"
#include "WinClientUART.hpp"
#else
#error "This program requires Linux or Win32."
#endif

#include "ErrorID.h"
#include "Global.hpp"
#include "GPINCommand.hpp"
#include "GPRMCommand.hpp"
#include "GVERCommand.hpp"
#include "ProgramVersion.hpp"
#include "ScanSequence.hpp"
#include "Scan2Sequence.hpp"
#include "ScanPrint.hpp"
#include "Scan2Print.hpp"
#include "SPRMCommand.hpp"
#include "KbhitGetch.h"

/**
 * Shows how to read the firmware version.
 * It is recommended to start any communication with this command.
 */
void
testGVER(IDataStream& theDataStream)
{
    GVERCommand lGVERCommand(theDataStream);
    ErrorID_t result = ERR_SUCCESS;

    printf("GVER: requesting firmware version...\r\n");
    result = lGVERCommand.performCommand();
    if (ERR_SUCCESS == result)
    {
        printf("GVER: Firmware version: %s\r\n", lGVERCommand.getVersion());
    }
    else
    {
        fprintf(stderr, "GVER: Error %d\r\n", result);
    }
    printf("\r\n");
}

/**
 * Shows how to read a parameter from the sensor.
 */
void
testGPRM(IDataStream& theDataStream)
{
    GPRMCommand lGPRMCommand(theDataStream);
    int32_t lParameterID = 0;
    int32_t lParameterValue = 0;
    ErrorID_t result = ERR_SUCCESS;

    printf("GPRM\r\n> Parameter ID?\r\n");
    fscanf(stdin, "%d", &lParameterID);

    printf("GPRM: requesting parameter %d...\r\n", lParameterID);
    result = lGPRMCommand.performCommand(lParameterID, lParameterValue);
    if (ERR_SUCCESS == result)
    {
        printf("GPRM: parameter %d: %d\r\n", lParameterID, lParameterValue);
    }
    else
    {
        fprintf(stderr, "GPRM: Error %d\r\n", result);
    }
    printf("\r\n");
}

/**
 * Shows how to program a sensor parameter.
 */
void
testSPRM(IDataStream& theDataStream)
{
    GPINCommand lGPINCommand(theDataStream);
    SPRMCommand lSPRMCommand(theDataStream);
    int32_t lParameterID = 0;
    int32_t lParameterValue = 0;
    ErrorID_t result = ERR_SUCCESS;

    // first, create a list of all parameters
    printf("GPIN: requesing parameter information...\r\n");
    lParameterID = 1;
    while (ERR_SUCCESS == lGPINCommand.performCommand(lParameterID))
    {
        printf("Parameter %8d; %8d; %s\r\n", lParameterID,
                lGPINCommand.getValue(), lGPINCommand.getInformation());
        lParameterID++;
    }

    printf("SPRM\r\n> Parameter ID (0 to return):\r\n");
    fscanf(stdin, "%d", &lParameterID);
    if (0 == lParameterID)
    {
        return;
    }

    printf("> New parameter value:\r\n");
    fscanf(stdin, "%d", &lParameterValue);

    result = lSPRMCommand.performCommand(lParameterID, lParameterValue);
    if (ERR_SUCCESS == result)
    {
        printf("SPRM: Parameter %d actually set to %d\r\n", lParameterID,
                lParameterValue);
    }
    else
    {
        fprintf(stderr, "SPRM: Error %d\r\n", result);
    }
    printf("\r\n");
}

/**
 * Shows how to control the scan process.
 * The example asks for a number of seconds to scan,
 * collects scans by use of GSCN and do some Test evaluations with the data.
 */
void
testSCAN(IDataStream& theDataStream, FILE* theTerminalLogFile)
{
    ScanSequence lScanSequence(theDataStream);
    lScanSequence.setTerminalLogFile(theTerminalLogFile);
    lScanSequence.run();
}

/**
 */
void
testSCANPR(IDataStream& theDataStream, FILE* theTerminalLogFile)
{
    ScanPrint lScanPrint(theDataStream);
    lScanPrint.setTerminalLogFile(theTerminalLogFile);
    lScanPrint.run();
}

/**
 */
void
testSCN2(IDataStream& theDataStream, FILE* theTerminalLogFile)
{
    Scan2Sequence lScanSequence(theDataStream);
    lScanSequence.setTerminalLogFile(theTerminalLogFile);
    lScanSequence.run();
}

/**
 */
void
testSCN2PR(IDataStream& theDataStream, FILE* theTerminalLogFile)
{
    Scan2Print lScan2Print(theDataStream);
    lScan2Print.setTerminalLogFile(theTerminalLogFile);
    lScan2Print.run();
}

/**
 */
void
testRELAY(IDataStream& theUART, IDataStream& theSocket, FILE* theTerminalLogFile)
{
	int32_t read_len;
	char read_data[64 * 1024];
	bool flag_GSC2 = false;
	int32_t param_GSC2_AvgNumber = 1;

    // terminal mode change on linux for kbhit of isTerminated().
    changemode(1);

    do
	{
		// if new data is available on the serial port, print it out
		if ((read_len = theUART.read(read_data, sizeof(read_data))) > 0)
		{
			flag_GSC2 = false;
			if (!strncmp(read_data, "GSC2", 4))
			{
		        CRC32 lCRC;
		        cast_ptr_t lBufferPtr = { read_data }; // casts the buffer to an array of integer
		        int32_t lCRCPosition = (4 + 4 + 4 + 4) / sizeof(int32_t) - 1; // position of the CRC in the buffer
		        uint32_t lCRCValue;
		        int32_t lLength;
		        int32_t lScanNumber;
		        int32_t lAvgNumber;

		        lLength = ntohl(lBufferPtr.asIntegerPtr[1]);
		        lScanNumber = ntohl(lBufferPtr.asIntegerPtr[2]);
		        param_GSC2_AvgNumber = lAvgNumber = ntohl(lBufferPtr.asIntegerPtr[3]);
#if 0
		        printf("GSC2: Length = %d\r\n", lLength);
		        printf("GSC2: ScanNumber = %d\r\n", lScanNumber);
		        printf("GSC2: AvgNumber = %d\r\n", lAvgNumber);
#endif
				strncpy((char *)lBufferPtr.asIntegerPtr + 0, "GSCN", 4);
				lBufferPtr.asIntegerPtr[1] = htonl(4); // Length
				lBufferPtr.asIntegerPtr[2] = htonl(lScanNumber); // Scan Number
				lCRCValue = lCRC.get(lBufferPtr.asIntegerPtr + 0, (4 + 4 + 4 + 4) - 4);
		        lBufferPtr.asIntegerPtr[lCRCPosition] = htonl(lCRCValue);
		        read_len = (4 + 4 + 4 + 4);
		        if (lAvgNumber > 1)
		        	flag_GSC2 = true;
			}
			theSocket.write(read_data, read_len);
		}
		// if new data is available on the console, send it to the serial port
		if ((read_len = theSocket.read(read_data, sizeof(read_data))) > 0)
		{
			while (flag_GSC2 && !strncmp(read_data, "GSCN", 4))
			{
			    CRC32 lCRC;
			    // type safe cast. CRC in network byte order is expected in the last 4 bytes of the buffer
			    cast_ptr_t lCRCReceivedPtr = { read_data + (read_len - 4) };
			    uint32_t lCRCExpected = ntohl(*lCRCReceivedPtr.asIntegerPtr);
			    uint32_t lCRCReceived = lCRC.get(read_data, read_len - 4);

#if 0
			    printf("GSC2: CRC expected:0x%x, received:0x%x\n", lCRCExpected, lCRCReceived);
#endif
			    if (lCRCExpected != lCRCReceived)
			    {
			    	break;
			    }

				// set moving integer pointer; skip command ID and length
				cast_ptr_t lDataPtr = { read_data };
				int32_t* lRcvIntegerPtr = &lDataPtr.asIntegerPtr[1];
				int32_t lLength;
				int32_t lSendLength;
				// take length of parameter block
				int32_t lNumberOfParameter;
				int32_t lNumberOfParameterExpected;
				int32_t lNumberOfEchoes = 0;
				int32_t lDataContent = 0;
				int32_t lNumberOfPoints = 0;
				int32_t lSendNumberOfPoints = 0;

				lLength = ntohl(*lRcvIntegerPtr++);
#if 0
				printf("Length = %d\r\n", lLength);
#endif

				lNumberOfParameter = ntohl(*lRcvIntegerPtr++);
				// check compatibility of firmware and control program
				if (lNumberOfParameter > 10 /*NUMBER_OF_SCAN_PARAMETER*/)
				{
					lNumberOfParameterExpected = 10 /*NUMBER_OF_SCAN_PARAMETER*/;
				}
				else
				{
					lNumberOfParameterExpected = lNumberOfParameter;
				}
#if 0
				printf("Number of parameters = %d, %d\r\n", lNumberOfParameter, lNumberOfParameterExpected);
#endif

				// copy known parameter to scan
				for (int32_t l = 0; l < lNumberOfParameterExpected; l++)
				{
#if 0
					printf("parameter[%d] = %d(0x%x)\r\n", l, ntohl(*lRcvIntegerPtr), ntohl(*lRcvIntegerPtr));
#endif
					if (l == 4 /* PARAMETER_NUMBER_OF_ECHOES */)
					{
						lNumberOfEchoes = ntohl(*lRcvIntegerPtr);
					}
					if (l == 8 /* PARAMETER_DATA_CONTENT */)
					{
						lDataContent = ntohl(*lRcvIntegerPtr);
					}
					lRcvIntegerPtr++;
				}

				// skip unkown parameter
				for (int32_t l = lNumberOfParameterExpected; l < lNumberOfParameter; l++)
				{
#if 0
					printf("parameter[%d] = %d(0x%x)\r\n", l, ntohl(*lRcvIntegerPtr), ntohl(*lRcvIntegerPtr));
#endif
					lRcvIntegerPtr++;
				}

				// get number of echoes. If 0, then the master echo is transfered instead of the number
#if 0
				printf("Data Content = %d\r\n", lDataContent);
#endif

#if 0
				printf("Number of echoes = %d\r\n", lNumberOfEchoes);
#endif
				if (0 == lNumberOfEchoes)
				{
					lNumberOfEchoes = 1;
				}

				// take number of points, check limits
				lNumberOfPoints = ntohl(*lRcvIntegerPtr++);
#if 0
				printf("Number of points = %d\r\n", lNumberOfPoints);
#endif

				if ((4 /* MAX_NUMBER_OF_ECHOS */ < lNumberOfEchoes) || (4000 /* MAX_POINTS_PER_SCAN */ < lNumberOfPoints))
				{
					break;
				}

				// copy data block according to the data content.
				if (1 /* lDataContent == 4 */ /* DATABLOCK_WITH_DISTANCES */)
				{
		        	int32_t lCnt = 0;
		        	int32_t lDistanceSum = 0;
		        	int32_t lSumCnt = 0;
					int32_t* lSendIntegerPtr = lRcvIntegerPtr;

		        	for (int32_t lPoints = 0; lPoints < lNumberOfPoints; lPoints++)
					{
						// loop for each point through all echos
						for (int32_t lEchos = 0; lEchos < lNumberOfEchoes; lEchos++)
						{
		                	int32_t lDistance;

		                	lDistance = ntohl(*lRcvIntegerPtr++);
		                	if (lDataContent != 4)
		                	{
		                		lRcvIntegerPtr++;
		                	}

		            		lCnt ++;

		            		if ((uint32_t)lDistance == 0x80000000)
		                	{
		                		if (lSumCnt == 0)
		                		{
		                			lDistanceSum = lDistance;
		                		}
		                	}
		                	else if ((uint32_t)lDistance == 0x7FFFFFFF)
		                	{
		                		if (lSumCnt == 0)
		                		{
		                			lDistanceSum = lDistance;
		                		}
		                	}
		                	else
		                	{
		                		if (lSumCnt == 0)
		                		{
		                			lDistanceSum = 0;
		                		}
		                		lSumCnt ++;
		                		lDistanceSum += lDistance;
		                	}

		                	if (lCnt == param_GSC2_AvgNumber)
		                	{
		                		if (lSumCnt > 0)
		                		{
			                		*lSendIntegerPtr++ = htonl(lDistanceSum / lSumCnt);
#if 0
			                		printf("%4d:%2d:", lPoints, lSumCnt);
		                			for (int32_t lSpaceCnt = 0; lSpaceCnt < ((lDistanceSum / lSumCnt) - 15000) / 500; lSpaceCnt ++)
		                			{
		                				printf("-");
		                			}
		            				printf("|\r\n");
#endif
		                		}
		                		else
		                		{
			                		*lSendIntegerPtr++ = htonl(lDistanceSum);
#if 0
		                			printf("%4d:%2d:%-8s\r\n", lPoints, lSumCnt, (uint32_t)lDistanceSum == 0x80000000 ? "Low" : "Noise");
#endif
		                		}

		                		if (lDataContent != 4)
			                	{
			                		lSendIntegerPtr++;
			                	}
			                	lSendNumberOfPoints ++;

			                	lCnt = 0;
		                		lDistanceSum = 0;
		                		lSumCnt = 0;
		                	}
						} // end echos
		                if ((lPoints == (lNumberOfPoints - 1)) && (lCnt != 0))
		            	{
		            		if (lSumCnt > 0)
		            		{
		                		*lSendIntegerPtr++ = htonl(lDistanceSum / lSumCnt);
#if 0
		            			printf("%4d:%2d:", lPoints, lSumCnt);
		            			for (int32_t lSpaceCnt = 0; lSpaceCnt < ((lDistanceSum / lSumCnt) - 15000) / 500; lSpaceCnt ++)
		            			{
		            				printf("-");
		            			}
		        				printf("|\r\n");
#endif
		            		}
		            		else
		            		{
		                		*lSendIntegerPtr++ = htonl(lDistanceSum);
#if 0
		            			printf("%4d:%2d:%-8s\r\n", lPoints, lSumCnt, (uint32_t)lDistanceSum == 0x80000000 ? "Low" : "Noise");
#endif
		            		}

		            		if (lDataContent != 4)
		                	{
		                		lSendIntegerPtr++;
		                	}
		                	lSendNumberOfPoints ++;
		            	}
					} // end points
				}

		        int32_t lCRCPosition; // position of the CRC in the buffer
		        uint32_t lCRCValue;

		        lSendLength = 4 + 4 * lNumberOfParameter + 4 + (lDataContent == 4 ? 4 : 8) * lSendNumberOfPoints;
		        lCRCPosition = (8 + lSendLength + 4) / sizeof(int32_t) - 1;
				lDataPtr.asIntegerPtr[1] = htonl(lSendLength); // Length
				lDataPtr.asIntegerPtr[12] = htonl(lSendNumberOfPoints); // Number of points
				lCRCValue = lCRC.get(lDataPtr.asIntegerPtr + 0,  8 + lSendLength);
				lDataPtr.asIntegerPtr[lCRCPosition] = htonl(lCRCValue);
		        read_len = 8 + lSendLength + 4;
#if 0
		        printf("Send Length = %d\r\n", lSendLength);
				printf("Send Number of points = %d\r\n", lSendNumberOfPoints);
				printf("Send CRCPosition = %d\r\n", lCRCPosition);
				printf("Send CRCValue = 0x%%x\r\n", lCRCValue);
#endif
		        break;
			}
			theUART.write(read_data, read_len);
		}
	} while((kbhit() == 0) || (getch() != 'q'));

    // terminal mode restore on linux for kbhit of isTerminated().
    changemode(0);
}

/*
 * Main for a number of tests.
 */
int
main(int argc, char **argv)
{
    ClientSocket lClientSocket;
    ClientUART lClientUART;
    IDataStream *lpDataSteam;
    bool done = false;
    int32_t lClientPort = 1025;
    int32_t lSelection = 0;
    int32_t lServerPort = 1024;
    string32_t lClientName = { "10.0.10.0" }; // you may enter here your default client IP, e.g. "10.0.3.12"
    string32_t lServerName = { "10.0.8.86" }; // default sensor IP
    string32_t lLogFileName = { 0 };
    string32_t lTerminalLogFileName = { 0 };
#if __WIN32__
    string32_t lUARTName = { "COM4" }; // you may enter here your default client UART port.
#else // if __linux__
    string32_t lUARTName = { "/dev/ttyO1" }; // you may enter here your default client UART port.
#endif
    FILE* lLogFile = 0;
    FILE* lTerminalLogFile = 0;

    // In Eclipse CDT on Windows, standard output of the program being run or
    // debugged is not connected to a Windows console, but to a pipe.
    // Add the following lines in the start of the main function:
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    // print program version
    printf("%s\r\n", getVersionString());

    // parse command line: help
    if (argc < 2 || (strcmp(argv[1], "NET") && strcmp(argv[1], "UART") && strcmp(argv[1], "RELAY")))
    {
		puts(	"Usage: PSDemoProgram \r\n"
				"   NET\r\n"
				"   [sensor_ip_address] [sensor_port]\r\n"
				"   [client_ip_address] [client_port]\r\n"
				"   [terminal log file] [data log file]\r\n"
				"   -or-\r\n"
				"   UART\r\n"
				"   [UART_port]\r\n"
				"   [terminal log file] [data log file]\r\n"
				"   -or-\r\n"
				"   RELAY\r\n"
				"   [UART_port]\r\n"
				"   [sensor_ip_address] [sensor_port]\r\n"
				"   [terminal log file] [data log file]\r\n\n");
		printf(	"Example:\r\n");
		printf(	"   PSTestProgram NET 10.0.3.12 1024 10.0.10.0 1025 term.txt scans.dat\r\n"
				"   -or- PSTestProgram NET 10.0.3.12\r\n");
		printf(	"   PSTestProgram UART %s terminal.txt scans.dat\r\n", lUARTName);
		printf(	"   -or- PSTestProgram UART %s\r\n", lUARTName);
		printf(	"   PSTestProgram RELAY %s 10.0.3.12 1024 10.0.10.0 1025 term.txt scans.dat\r\n", lUARTName);
		printf(	"   -or- PSTestProgram RELAY %s 10.0.3.12 terminal.txt scans.dat\r\n", lUARTName);
		return -1;
	}

    if (!strcmp(argv[1], "NET") || !strcmp(argv[1], "UART"))
    {
		if (!strcmp(argv[1], "NET"))
		{
			// get sensor IP and port from the command line
			if (3 <= argc)
			{
				strcpy(lServerName, argv[2]);
			}
			if (4 <= argc)
			{
				lServerPort = atoi(argv[3]);
			}

			// get sensor and local IP from the command line
			if (5 <= argc)
			{
				strcpy(lClientName, argv[4]);
			}
			if (6 <= argc)
			{
				lClientPort = atoi(argv[5]);
			}

			// open the log files
			if (7 <= argc)
			{
				strcpy(lTerminalLogFileName, argv[6]);
				lTerminalLogFile = fopen(lTerminalLogFileName, "ab");
			}
			if (8 <= argc)
			{
				strcpy(lLogFileName, argv[7]);
				lLogFile = fopen(lLogFileName, "wb");
			}

			printf("Sensor IP: %s:%d\r\nLocal IP: %s:%d\r\n\n", lServerName,
					lServerPort, lClientName, lClientPort);

			// create the socket
			lClientSocket.setClientIPAddress(lClientName, lClientPort);
			lClientSocket.setServerIPAddress(lServerName, lServerPort);
			lClientSocket.setLogFile(lLogFile);
			lClientSocket.setTimeout(10);

			// open the socket
			if (ERR_SUCCESS != lClientSocket.open())
			{
				if (lLogFile) fclose(lLogFile);
				if (lTerminalLogFile) fclose(lTerminalLogFile);
				fprintf(stderr, "Error: Cannot open sensor connection!\r\n");
				return ERR_IO;
			}
			lpDataSteam = &lClientSocket;
		}
		else // if (!strcmp(argv[1], "UART"))
		{
			// get UART port from the command line
			if (argc >= 3)
			{
				strcpy(lUARTName, argv[2]);
			}

			// open the log files
			if (argc >= 4)
			{
				strcpy(lTerminalLogFileName, argv[3]);
				lTerminalLogFile = fopen(lTerminalLogFileName, "ab");
			}
			if (argc >= 5)
			{
				strcpy(lLogFileName, argv[4]);
				lLogFile = fopen(lLogFileName, "wb");
			}

			printf("UART port: %s\r\n\n", lUARTName);

			// configure the UART
			lClientUART.config(lUARTName, 10, lLogFile);

			// open the UART
			if (ERR_SUCCESS != lClientUART.open())
			{
				if (lLogFile) fclose(lLogFile);
				if (lTerminalLogFile) fclose(lTerminalLogFile);
				fprintf(stderr, "Error: Cannot open UART connection!\r\n");
				return ERR_IO;
			}
			lpDataSteam = &lClientUART;
		}

		// menu
	    while (false == done)
	    {
	        printf("\r\nPS Demo Program:\r\n"
	                " 1 - GVER: get firmware version\r\n"
	                " 2 - GPRM: Getting a sensor parameter\r\n"
	                " 3 - SPRM: Setting a sensor parameter\r\n"
	                " 4 - SCAN: Starting a scan sequence\r\n"
	                " 5 - SCANPR: Getting a scan data and print graph\r\n");
	        if (!strcmp(argv[1], "UART"))
	        {
	        printf( " 6 - SCN2: Starting a scan2 sequence\r\n"
	                " 7 - SCN2PR: Getting a scan2 data and print graph\r\n");
	        }
	        printf( " 0 - Exit\r\n> ");
	        fscanf(stdin, "%d", &lSelection);

	        switch (lSelection)
	        {
	            case 1:
	                testGVER(*lpDataSteam);
	                break;

	            case 2:
	                testGPRM(*lpDataSteam);
	                break;

	            case 3:
	                testSPRM(*lpDataSteam);
	                break;

	            case 4:
	                testSCAN(*lpDataSteam, lTerminalLogFile);
	                break;

	            case 5:
	                testSCANPR(*lpDataSteam, lTerminalLogFile);
	                break;

	            case 6:
	                testSCN2(*lpDataSteam, lTerminalLogFile);
	                break;

	            case 7:
	                testSCN2PR(*lpDataSteam, lTerminalLogFile);
	                break;

	            case 0:
	                done = true;
	                break;

	            default:
	                // ignore
	                break;
	        }
	    }
    }
    else //if (!strcmp(argv[1], "RELAY"))
    {
		// get UART port from the command line
		if (argc >= 3)
		{
			strcpy(lUARTName, argv[2]);
		}

		printf("UART port: %s\r\n\n", lUARTName);

		// get sensor IP and port from the command line
		if (4 <= argc)
		{
			strcpy(lServerName, argv[3]);
		}
		if (5 <= argc)
		{
			lServerPort = atoi(argv[4]);
		}

		// get sensor and local IP from the command line
		if (6 <= argc)
		{
			strcpy(lClientName, argv[5]);
		}
		if (7 <= argc)
		{
			lClientPort = atoi(argv[6]);
		}

		// open the log files
		if (8 <= argc)
		{
			strcpy(lTerminalLogFileName, argv[7]);
			lTerminalLogFile = fopen(lTerminalLogFileName, "ab");
		}
		if (9 <= argc)
		{
			strcpy(lLogFileName, argv[8]);
			lLogFile = fopen(lLogFileName, "wb");
		}

		printf("Sensor IP: %s:%d and Local IP: %s:%d\r\n\n", lServerName, lServerPort, lClientName, lClientPort);

		// configure the UART
		lClientUART.config(lUARTName, 0, lLogFile);

		// open the UART
		if (ERR_SUCCESS != lClientUART.open())
		{
			if (lLogFile) fclose(lLogFile);
			if (lTerminalLogFile) fclose(lTerminalLogFile);
			fprintf(stderr, "Error: Cannot open UART connection!\r\n");
			return ERR_IO;
		}

		// create the socket
		lClientSocket.setClientIPAddress(lClientName, lClientPort);
		lClientSocket.setServerIPAddress(lServerName, lServerPort);
		lClientSocket.setLogFile(lLogFile);
		lClientSocket.setTimeout(0);

		// open the socket
		if (ERR_SUCCESS != lClientSocket.open())
		{
			if (lLogFile) fclose(lLogFile);
			if (lTerminalLogFile) fclose(lTerminalLogFile);
			fprintf(stderr, "Error: Cannot open sensor IP connection!\r\n");
			return ERR_IO;
		}

        testRELAY(lClientUART, lClientSocket, lTerminalLogFile);
    }

    // close the socket
    if(lClientSocket.isOpen())
    {
    	lClientSocket.close();
    }
    // close the UART
    if(lClientUART.isOpen())
    {
    	lClientUART.close();
    }
    if (lLogFile) fclose(lLogFile);
    if (lTerminalLogFile) fclose(lTerminalLogFile);
    printf("Bye-bye.\r\n");
    return 0;
}
