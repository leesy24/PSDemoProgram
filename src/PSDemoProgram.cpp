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
#include "ScanPrint.hpp"
#include "SPRMCommand.hpp"

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
testSCPR(IDataStream& theDataStream, FILE* theTerminalLogFile)
{
    ScanPrint lScanPrint(theDataStream);
    lScanPrint.setTerminalLogFile(theTerminalLogFile);
    lScanPrint.run();
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
    string32_t lUARTName = { "COM4" }; // you may enter here your default client UART port.
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
    if (argc < 2 || (strcmp(argv[1], "NET") && strcmp(argv[1], "UART")))
    {
        puts("Usage: PSDemoProgram \r\n"
                "   NET\r\n"
                "   [sensor_ip_address] [sensor_port]\r\n"
                "   [client_ip_address] [client_port]\r\n"
                "   [terminal log file] [data log file]\r\n"
                "   -or-\r\n"
				"   UART\r\n"
				"   [UART_port]\r\n"
				"   [terminal log file] [data log file]\r\n");
        puts(
                "Example:\r\n"
                        "  PSTestProgram NET 10.0.3.12 1024 10.0.10.0 1025 terminal.txt scans.dat\r\n"
        				"  PSTestProgram UART COM4 terminal.txt scans.dat\r\n");
        return -1;
    }

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
    else //if (!strcmp(argv[1], "UART"))
    {
		// get sensor IP and port from the command line
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

		printf("Sensor UART: %s\r\n\n", lUARTName);

		// configure the UART
		lClientUART.config(lUARTName, 10, lLogFile);

		// open the UART
		if (ERR_SUCCESS != lClientUART.open())
		{
			if (lLogFile) fclose(lLogFile);
			if (lTerminalLogFile) fclose(lTerminalLogFile);
			fprintf(stderr, "Error: Cannot open sensor connection!\r\n");
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
                " 5 - SCPR: Getting a scan data and print graph\r\n\n"
                " 0 - Exit\r\n> ");
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
                testSCPR(*lpDataSteam, lTerminalLogFile);
                break;

            case 0:
                done = true;
                break;

            default:
                // ignore
                break;
        }
    }

    // close the socket
    lpDataSteam->close();
    if (lLogFile) fclose(lLogFile);
    if (lTerminalLogFile) fclose(lTerminalLogFile);
    printf("Bye-bye.\r\n");
    return 0;
}
