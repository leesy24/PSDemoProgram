/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ScanSequence.cpp
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

#include "ScanSequence.hpp"
#include "IDataStream.hpp"

/*
 * Standard constructor.
 */
ScanSequence::ScanSequence(IDataStream& theDataStream) :
        mComputerSendTimeCode(0), //
        mComputerReceiveTimeCode(0), //
        mGSCNCommand(theDataStream), //
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
ScanSequence::~ScanSequence()
{
    // nothing to do.
}

/*
 * Returns the current system time in ms.
 */
unsigned int
ScanSequence::getMilliseconds()
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
ScanSequence::getTime()
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
ScanSequence::isTerminated()
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
ScanSequence::logTableHeader()
{
    string128_t lTextLine;

    // format
    sprintf(lTextLine, "\r\n%14s ; %12s; %12s;  %8s;  %8s; %8s;\r\n", //
            "Time", "Scan", "PS Time", "d Scans", "d Cmds", "Lost scans");

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
ScanSequence::logMessage(const char* const theMessage)
{
    string128_t lTextLine;
    struct tm lLocalTime = getTime();
    lLocalTime = getTime();

    if (0 != theMessage)
    {
        // format
        sprintf(lTextLine, "%2d.%02d./%2d:%02d:%02d: %s\r\n", lLocalTime.tm_mday, lLocalTime.tm_mon + 1,
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
ScanSequence::logError(int theError)
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
ScanSequence::logScan()
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
        mScannerTimeCode = mScan.mParameter[GSCNCommand::PARAMETER_TIME_STAMP];
        mScanNumber = mScan.mParameter[GSCNCommand::PARAMETER_SCAN_NUMBER];
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
            sprintf(lTextLine,
                    "%2d.%02d./%2d:%02d:%02d; %12d; %8.3f [s]; %4d [ms]; %4d [ms]; %4d;\r\n", //
                    lLocalTime.tm_mday, lLocalTime.tm_mon + 1, lLocalTime.tm_hour, lLocalTime.tm_min, lLocalTime.tm_sec,
                    mScanNumber, TIME_TO_SECONDS(mScannerTimeCode), //
                    mScannerTimeCode - lLastScannerTimeCode, mComputerReceiveTimeCode - mComputerSendTimeCode,
                    lLostScans);
        }
    } // end valid scan

    // empty scan
    else
    {
        sprintf(lTextLine,
                "%2d.%02d./%2d:%02d:%02d; %12d; %12s;\r\n", //
                lLocalTime.tm_mday, lLocalTime.tm_mon + 1, lLocalTime.tm_hour, lLocalTime.tm_min, lLocalTime.tm_sec,
                mNumberOfScans, "Empty scan");
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
ScanSequence::processScan()
{
    // not an empty scan?
    if (0 != mScan.mNumberOfParameter)
    {
        // example code: show some profile infos.
        printf(
                "Scan %d; time %d; Incremental %d; ScanLine %d\r\r", //
                mScan.mParameter[GSCNCommand::PARAMETER_SCAN_NUMBER],
                mScan.mParameter[GSCNCommand::PARAMETER_TIME_STAMP],
                mScan.mParameter[GSCNCommand::PARAMETER_INCREMENTAL_ENCODER],
                mScan.mParameter[GSCNCommand::PARAMETER_SCAN_LINE]);
    } // end valid scan

    return ERR_SUCCESS;
}

/*
 * Let the scan process run.
 */
ErrorID_t
ScanSequence::run()
{
    ErrorID_t result = ERR_SUCCESS;

    // reset
    mScanNumber = 0;
    mScannerTimeCode = 0;
    mNumberOfScans = 0;

    // ask for the terminating condition
    result = setTerminateCondition();
    if (ERR_SUCCESS == result)
    {
        // SCAN command: start the scan sequence
        logMessage("Starting Scan...\r\n");
        result = mSCANCommand.performCommand();

        // prepare table
        logTableHeader();

        // loop until the scans terminate.
        while (false == isTerminated())
        {
            // request a scan, measure how long this takes on this machine.
            mNumberOfScans++;
            mComputerSendTimeCode = getMilliseconds();
            result = mGSCNCommand.performCommand(0, mScan);
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
        } // end while not terminated
    } // end terminate condition OK.
    return result;
}

/*
 * Example code: Define the condition to terminate the scan process.
 * Here: asks for a period of time to let the scanner run.
 */
ErrorID_t
ScanSequence::setTerminateCondition()
{
    int32_t lScanPeriod = 0;

    // enter the scan period
    fprintf(stdout, "> Scan period [s] (0=infinitive): ");
    fscanf(stdin, "%d", &lScanPeriod);

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
ScanSequence::setTerminalLogFile(FILE* theTerminalLogName)
{
    mTerminalLogFile = theTerminalLogName;
}

