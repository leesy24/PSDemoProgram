#ifndef SCANSEQUENCE_HPP_
#define SCANSEQUENCE_HPP_

/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       ScanSequence.hpp -
 *              Base class and example how to build a scan sequence.
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
 ****************************************************************************
 * Last changes:
 * 24.04.2014 CBD:
 * - Changed: use IDataStream for communication.
 * - New: create a shorter table.
 */

#include "Global.hpp"
#include "GSCNCommand.hpp"
#include "SCANCommand.hpp"
#include "ErrorID.h"
class IDataStream;

/**
 * Class ScanSequence.hpp - Base class for scan sequences.
 * <p>&nbsp;<p>
 * This is a base class and an example to handle a sequence of scans.
 * The example starts the scan process by use of a SCAN command
 * and collects scan profiles by GSCN.
 * <p>
 * The example counts the number of invalid echos and check the profile
 * number for lost profiles.
 *
 * <b>Eclipse IDE</b>
 * The sequence is terminated if the user presses the ESC key.
 * In Eclipse CDT on Windows, standard output of the program being run or
 * debugged is not connected to a Windows console, but to a pipe.
 * Since kbhit() will not work in the IDE, terminate the sequence by time.
 *
 *  <p>&nbsp;<p>
 *
 */
class ScanSequence
{
        /** class constants */
        enum ScanSequenceConstants_t
        {
            /** show results of each nth scan on the console */
            DEBUG_LOG_INTERVAL = 10,

            /** show results of each nth scan on the console */
            RELEASE_LOG_INTERVAL = 1000,

            /** if this flag is set shows the example lost scans only */
            SHOW_LOST_SCANS_ONLY = 1,

            /** distance is this value in case of noise/too strong echoes */
            NOISE = INT32_MAX,

            /**
             * Distance is this value in case of low echo.
             * Check the pulse width: it is 0 in case of no echo.
             */
            LOW_DISTANCE = INT32_MIN,

            /**
             * ESC key
             */
            VK_ESC = 27
        };

        /** computer time stamp when the command was send */
        int32_t mComputerSendTimeCode;

        /** computer time stamp when all data are received */
        int32_t mComputerReceiveTimeCode;

        /** the GSCN command */
        GSCNCommand mGSCNCommand;

        /** Number of scans requested by the computer */
        int32_t mNumberOfScans;

        /** the scan data */
        GSCNCommand::Scan_t mScan;

        /** the SCAN command */
        SCANCommand mSCANCommand;

        /** time code of the latest scan */
        uint32_t mScannerTimeCode;

        /** number of the latest scan */
        int32_t mScanNumber;

        /** ms the test should run */
        unsigned int mScanPeriod;

        /** ms when the sequence was started */
        unsigned int mStartTime;

        /** the terminal log file */
        FILE* mTerminalLogFile;

        /**
         * Returns the current system time in ms.
         * Watch out for timer overflows.
         *
         * @return
         *      local time in ms.
         */
        static unsigned int
        getMilliseconds();

        /**
         * Returns the current computer time.
         *
         * @return
         *      a struct tm with the local computer time.
         */
        struct tm
        getTime();

        /* private assignment constructor to avoid misuse */
        explicit
        ScanSequence(const ScanSequence&);

        /* private assignment operator to avoid misuse */
        ScanSequence&
        operator =(const ScanSequence&);

    protected:

        /**
         * Example code: Proofs the terminating condition.
         * Here: check for ESC or if the sequence has expired.
         * Note: in Eclipse IDE will ESC not work.
         */
        virtual bool
        isTerminated();

        /**
         * Writes the table header to a file or the console.
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        logTableHeader();

        /**
         * Log the scan in a file or on the console.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        logScan();

        /**
         * Logs the current time and a message in a file or on the console.
         *
         * @param theMessage
         *      String to be logged.
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        logMessage(const char* const theMessage);

        /**
         * Logs an error in a file or on the console.
         *
         * @param theError
         *      The error code to be logged; either of type ErrorID_t from the sensor
         *      or a negative error code from the socket driver.
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        logError(int theError);

        /**
         * Example code: Process the received data.
         * Here: filter and count invalid distances (no echo/noise),
         * print time codes and lost scans.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        processScan();

        /**
         * Example code: Define the condition to terminate the scan process.
         * Here: asks for a period of time to let the scanner run.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        virtual ErrorID_t
        setTerminateCondition();

    public:

        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to the sensor; must be opened before.
         */
        ScanSequence(IDataStream& theDataStream);

        /**
         * Mandatory C++ destructor.
         */
        virtual
        ~ScanSequence();

        /**
         * Starts the scan process.
         * Calls the virtual method processScan() to do something with the data.
         * Checks the virtual method isTermated()  to stop the scan
         * process.
         * Terminates if ESC was pressed on the keyboard.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        ErrorID_t
        run(char* theDataLogFileName);

        /**
         * Defines a text file to log the terminal output.
         * The file must be opened before.
         *
         * @param theTerminalLogName
         *      text file handle, 0 to close the terminal log file.
         */
        void
        setTerminalLogFile(FILE* theTerminalLogName);
};

#endif /* SCANSEQUENCE_HPP_ */
