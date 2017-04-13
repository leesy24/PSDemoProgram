/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GSCNCommand.hpp - Reading sensor parameters
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2016/01/13 13:34:52 $
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
 * Last changes:
 * 04.07.2015 CBD:
 * - New: GSCN parameter PARAMETER_SCAN_LINE
 * - Changed: read known GSCN parameter and skip extended ones.
 * - Removed: sendCommand()
 * Last changes:
 * 27.08.2015 CBD:
 * - Changed: accept 4 bytes as special data format, any other gives 8 byte according to ICD.
 */

#ifndef GSCNCOMMAND_HPP_
#define GSCNCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class GSCNCommand.hpp - Getting scans from the sensor.
 */
class GSCNCommand : private CommandBase
{
    public:

        /** GSCN Parameter block. Is transmitted exactly in this order. */
        enum GSCNParameterID_t
        {
            PARAMETER_SCAN_NUMBER, //
            PARAMETER_TIME_STAMP, //
            PARAMETER_SCAN_START_DIRECTION, //
            PARAMETER_SCAN_ANGLE, //
            PARAMETER_NUMBER_OF_ECHOES, //
            PARAMETER_INCREMENTAL_ENCODER, //
            PARAMETER_TEMPERATURE, //
            PARAMETER_SYSTEM_STATUS, //
            PARAMETER_DATA_CONTENT, //
            PARAMETER_SCAN_LINE, //
            NUMBER_OF_SCAN_PARAMETER, //

            /** data block is empty */
            NO_DATABLOCK = 0,

            /** data block contains distances only */
            DATABLOCK_WITH_DISTANCES = 4,

            /** data block contains distances and pulse witdh */
            DATABLOCK_WITH_DISTANCES_PW = 8,

            /** data block contains distances, and pulse witdh with echoes */
            DATABLOCK_WITH_DISTANCES_PW_INCLUDES_ECHO = 7,

            /** defines the max. number of points per profile */
            MAX_POINTS_PER_SCAN = 4000,

            /** defines the max. number of echoes per point */
            MAX_NUMBER_OF_ECHOS = 4
        };

        /** GSCN scan Type definition. */
        struct Scan_t
        {
                /** number of parameter in the parameter array */
                int32_t mNumberOfParameter;

                /** scan parameter array */
                int32_t mParameter[NUMBER_OF_SCAN_PARAMETER];

                /** number of points in the scan*/
                int32_t mNumberOfPoints;

                /** number of echos in the scan*/
                int32_t mNumberOfEchoes;

                /** array with distances + pulse width */
                struct
                {
                        int32_t mDistance;
                        int32_t mPulseWidth;
                } mScanData[MAX_POINTS_PER_SCAN][MAX_NUMBER_OF_ECHOS];
        };

    private:

        /** GSCN command data to be sent to the sensor. */
        struct command_t
        {
                char mCommandID[4];
                int32_t mLength;
                int32_t mScanNumber;
                int32_t mCRC;
        } mCommand;

        /**
         * Parses the receiver buffer and copy the result into the scan structure.
         * Must be called after performCommand().
         *
         * @param theScan
         *      Structure of type Scan_t to store the results.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         *
         */
        ErrorID_t
        parseScan(Scan_t& theScan);

        /* private assignment constructor to avoid misuse */
        explicit
        GSCNCommand(const GSCNCommand& src);

        /* private assignment operator to avoid misuse */
        GSCNCommand&
        operator =(const GSCNCommand& src);

    public:

        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        GSCNCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~GSCNCommand();

        /**
         * Clears a Scan_t structure.
         */
        void
        clearScan(Scan_t& theScan);

        /**
         * Performs the GSCN command.
         *
         * @param theScanNumber
         *      the scan number; 0 means the latest scan measured.
         * @param theScan
         *      Structure of type Scan_t to store the received data.
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        ErrorID_t
        performCommand(int32_t theScanNumber, Scan_t& theScan, char* theDataLogFileName);
};

#endif /* GSCNCOMMAND_HPP_ */
