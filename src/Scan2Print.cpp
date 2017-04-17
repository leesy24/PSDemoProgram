/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       Scan2Print.cpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2016/01/15 10:35:56 $
 * SDK:         GNU gcc 4
 *
 * (c) 2014 Triple-IN GmbH Hamburg, Germany
 *
 * This software is placed into the public domain
 * and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 ****************************************************************************
 */
#include <ctime>
#include <cstdio>
#include <sys/time.h>
#include "KbhitGetch.h"

#include "IDataStream.hpp"
#include "Scan2Print.hpp"

#if __WIN32__
	#include <windows.h>
	#define sleep_msecs(s) Sleep(s)
#elif __linux__
	#include <unistd.h>
	#define sleep_msecs(s) usleep(s*1000)
#else
	#error "This program requires Linux or Win32."
#endif

/*
 * Standard constructor.
 */
Scan2Print::Scan2Print(IDataStream& theDataStream) :
        mComputerSendTimeCode(0), //
        mComputerReceiveTimeCode(0), //
        mGSC2Command(theDataStream), //
        mNumberOfScans(0), //
        mScan(), //
        mSCANCommand(theDataStream), //
        mScannerTimeCode(0), //
        mScanNumber(0), //
        mScanPeriod(0), //
        mStartTime(0), //
        mTerminalLogFile(0)
{
    // check the connection
    if (false == theDataStream.isOpen())
    {
        theDataStream.open();
    }
}

/*
 * Standard destructor.
 */
Scan2Print::~Scan2Print()
{
    // nothing to do.
}

/*
 * Returns the current system time in ms.
 */
unsigned int
Scan2Print::getMilliseconds()
{
    struct timeval tv;

    gettimeofday(&tv, NULL); // get current time
    const unsigned int milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000; // turn to milliseconds
    return milliseconds;
}

/*
 * Returns the current computer time.
 */
struct tm
Scan2Print::getTime()
{
    time_t lTime;

    time(&lTime);
    return *localtime(&lTime);
}

/*
 * Example code: Proofs the terminating condition.
 * here: ESC pressed on the keyboard?
 */
bool
Scan2Print::isTerminated()
{
	int key;

    // not started or expired
    bool lExpired = ((0 != mScanPeriod) && ((mStartTime + mScanPeriod) < getMilliseconds()));
    if ((0 == mStartTime) || lExpired)
    {
        mStartTime = 0; // set to "not started"
        return true;
    }

    // check for ESC or 'q' or 'Q'
    return ((0 != kbhit()) && (((key = getch()) == VK_ESC) || (key == 'q') || (key == 'Q')));
}

/*
 * Writes the table header to a file or the console.
 */
ErrorID_t
Scan2Print::logTableHeader()
{
    string128_t lTextLine;

    // format
    sprintf(lTextLine, "\r\n%14s %8s %8s %4s %4s %8s \r\n", //
            "Time", "Scan", "PS Time", "d-S", "d-C", "Lost-S");

    // write to file
    if (0 != mTerminalLogFile)
    {
        fputs(lTextLine, mTerminalLogFile);
    }

    // write to console
    puts(lTextLine);
    return ERR_SUCCESS;
}

/*
 * Logs a message in a file or on the console.
 */
ErrorID_t
Scan2Print::logMessage(const char* const theMessage)
{
    string128_t lTextLine;
    struct tm lLocalTime = getTime();
    lLocalTime = getTime();

    if (0 != theMessage)
    {
        // format
        sprintf(lTextLine, "%2d/%02d_%2d:%02d:%02d %s\r\n", lLocalTime.tm_mday, lLocalTime.tm_mon + 1,
                lLocalTime.tm_hour, lLocalTime.tm_min, lLocalTime.tm_sec, theMessage);

        // write to file
        if (0 != mTerminalLogFile)
        {
            fputs(lTextLine, mTerminalLogFile);
        }

        // write to console
        puts(lTextLine);
    }
    return ERR_SUCCESS;
}

/*
 * Logs an error message in a file or on the console.
 */
ErrorID_t
Scan2Print::logError(int theError)
{
    string32_t lMessage;
    if (0 != theError)
    {
        sprintf(lMessage, "%d\r\n", theError);
        return logMessage(lMessage);
    }
    return ERR_SUCCESS;

}

/*
 * Log the scan in a file or on the console.
 */
ErrorID_t
Scan2Print::logScan()
{
    // temp. copy of recent scan data
    string128_t lTextLine = { 0 };
    int32_t lLastScannerTimeCode = mScannerTimeCode;
    int32_t lLastScanNumber = mScanNumber;
    int32_t lLogInterval = DEBUG_LOG_INTERVAL;
    struct tm lLocalTime = getTime();

    // valid scan?
    if (0 != mScan.mNumberOfParameter)
    {
        mScannerTimeCode = mScan.mParameter[GSC2Command::PARAMETER_TIME_STAMP];
        mScanNumber = mScan.mParameter[GSC2Command::PARAMETER_SCAN_NUMBER];
        const int32_t lLostScans = mScanNumber - lLastScanNumber - 1;

        // this is the 1st scan.
        if (0 == lLastScanNumber)
        {
            lLastScannerTimeCode = mScannerTimeCode;
        }

        // print result
        if ((0 < lLostScans) // if there there are lost scans
        || (0 == (mScanNumber % lLogInterval)) // or regular interval has expired
                || (0 == lLastScanNumber)) // or 1st scan
        {
            for (int32_t lPoints = 0; lPoints < mScan.mNumberOfPoints; lPoints++)
            {
                // loop for each point through all echos
                for (int32_t lEchoes = 0; lEchoes < mScan.mNumberOfEchoes; lEchoes++)
                {
                	int32_t lDistance = mScan.mScanData[lPoints][lEchoes].mDistance;
                	//int32_t lPulseWidth = mScan.mScanData[lPoints][lEchoes].mPulseWidth;

            		if (((uint32_t)lDistance == 0x80000000) || ((uint32_t)lDistance == 0x7FFFFFFF))
                	{
               			printf("%4d:%-8s\r\n", lPoints, (uint32_t)lDistance == 0x80000000 ? "Low" : "Noise");
                	}
                	else
                	{
						//printf("%4d:%8d\r\n", lPoints, lDistanceSum / lSumCnt);
						printf("%4d:", lPoints);
						for (int32_t lSpaceCnt = 0; lSpaceCnt < (lDistance - 15000) / 500; lSpaceCnt ++)
						{
							printf("-");
						}
						printf("|\r\n");
                	}
                } // end echos
            } // end points
        }
    } // end valid scan

    // empty scan
    else
    {
    }

    // log on screen and to file
    if (0 != lTextLine[0])
    {
        printf(lTextLine);
        if (0 != mTerminalLogFile)
        {
            fputs(lTextLine, mTerminalLogFile);
        }
    }
    return ERR_SUCCESS;
}

/*
 * Example code: Process the received data.
 */
ErrorID_t
Scan2Print::processScan()
{
    // not an empty scan?
    if (0 != mScan.mNumberOfParameter)
    {
        // example code: show some profile infos.
        printf(
                "Scan %d; time %d; Incremental %d; ScanLine %d \\ / \r\n", //
                mScan.mParameter[GSC2Command::PARAMETER_SCAN_NUMBER],
                mScan.mParameter[GSC2Command::PARAMETER_TIME_STAMP],
                mScan.mParameter[GSC2Command::PARAMETER_INCREMENTAL_ENCODER],
                mScan.mParameter[GSC2Command::PARAMETER_SCAN_LINE]);
    } // end valid scan

    return ERR_SUCCESS;
}

/*
 * Let the scan process run.
 */
ErrorID_t
Scan2Print::run(char* theDataLogFileName)
{
    ErrorID_t result = ERR_SUCCESS;

    // reset
    mScanNumber = 0;
    mScannerTimeCode = 0;
    mNumberOfScans = 0;

    // ask average value
    fprintf(stdout, "> Average count : ");
    fscanf(stdin, "%d", &mAvgNumber);
    if (mAvgNumber <= 0)
    {
    	mAvgNumber = 1;
    }

    // ask for the terminating condition
    result = setTerminateCondition();
    if (ERR_SUCCESS == result)
    {
        // SCAN command: start the scan sequence
        logMessage("Starting Scan... \\ /  \r\n");
        mSCANCommand.setParameter(1);
        result = mSCANCommand.performCommand();

        // prepare table
        logTableHeader();

        // terminal mode change on linux for kbhit of isTerminated().
        changemode(1);

        // loop until the scans terminate.
        while (false == isTerminated())
        {
            // request a scan, measure how long this takes on this machine.
            mNumberOfScans++;
            mComputerSendTimeCode = getMilliseconds();
            result = mGSC2Command.performCommand(0, mAvgNumber, mScan, theDataLogFileName);
            mComputerReceiveTimeCode = getMilliseconds();

            // process the scan data and show result
            if (ERR_SUCCESS == result)
            {
            	// WARNING "printf()" routine extensively used in this calls causes lost scans
                result = processScan();
                logScan();
            }

            // or process the error code
            if (ERR_SUCCESS != result)
            {
                logError(result);
            }

            // sleep 1000msec
            sleep_msecs(1000);
        } // end while not terminated

        // terminal mode restore on linux for kbhit of isTerminated().
        changemode(0);

        // SCAN command: end the scan sequence
        logMessage("End Scan... \\ /  \r\n");
        mSCANCommand.setParameter(0);
        result = mSCANCommand.performCommand();

    } // end terminate condition OK.
    return result;
}

/*
 * Example code: Define the condition to terminate the scan process.
 * Here: asks for a period of time to let the scanner run.
 */
ErrorID_t
Scan2Print::setTerminateCondition()
{
    int32_t lScanPeriod = 0;

    // enter the scan period
    //fprintf(stdout, "> Scan period [s] (0=infinitive): ");
    //fscanf(stdin, "%d", &lScanPeriod);

    // convert to ms
    mScanPeriod = lScanPeriod * 1000;

    // get the start time
    mStartTime = getMilliseconds();
    return ERR_SUCCESS;
}

/*
 * Defines a text file to log the terminal output.
 */
void
Scan2Print::setTerminalLogFile(FILE* theTerminalLogName)
{
    mTerminalLogFile = theTerminalLogName;
}

