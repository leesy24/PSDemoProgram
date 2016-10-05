/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GPINCommand.hpp - Requesting parameter information.
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
 * Last changes:
 * 24.04.2014 CBD:
 * - Changed: use IDataStream for communication.
 */

#ifndef GPINCOMMAND_HPP_
#define GPINCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class GPINCommand.hpp - Requesting parameter information.
 *
 * <p>
 * <b>Executive summary:</b>
 * <p>
 * Requests parameter information from a sensor connected.
 *  <p>&nbsp;<p>
 *
 *
 * <b>OS/Hardware Dependencies</b>
 * <p>
 * Requires the winsock library, however, the code can easily be ported
 * to any other device.
 * <p>&nbsp;<p>
 */
class GPINCommand : private CommandBase
{
    private:

        /** Class constants */
        enum
        {
            /**
             * At this position in the receiver buffer
             * we expect the parameter info string.
             */
            GPIN_INFO_OFFSET = 32
        };

        /** the command data to be se
         * nt to the sensor. */
        struct
        {
                char mCommandID[4];
                int32_t mLength;
                int32_t mParameterID;
                int32_t mCRC;
        } mCommand;

        /** parameter ID */
        int32_t mParameterID;

        /** parameter information */
        char mParameterInformation[128];

        /** maximum value */
        int32_t mMaximumValue;

        /** minimum value */
        int32_t mMinimumValue;

        /** current value */
        int32_t mValue;

        /**
         * Clears the parameter.
         */
        void
        clearParameterInfo();

        /* private assignment constructor to avoid misuse */
        explicit
        GPINCommand(const GPINCommand& src);

        /* private assignment operator to avoid misuse */
        GPINCommand&
        operator =(const GPINCommand& src);

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        GPINCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~GPINCommand();

        /**
         * Return the info string received.
         *
         * @return
         *          the info string.
         */
        inline const char*
        getInformation()
        {
            return mParameterInformation;
        }

        /**
         * Return the current value.
         *
         * @return
         *      current parameter value.
         */
        inline int32_t
        getValue()
        {
            return mValue;
        }

        /**
         * Performs the GPIN command.
         *
         * @param theParameterID
         *      ID of the parameter to be obtained.
         */
        ErrorID_t
        performCommand(int32_t theParameterID);
};

#endif /* GPINCOMMAND_HPP_ */
