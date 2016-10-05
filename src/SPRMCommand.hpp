/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       SPRMCommand.hpp - Setting sensor parameters
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

#ifndef SPRMCOMMAND_HPP_
#define SPRMCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class SPRMCommand.hpp - Setting a parameter from the sensor.
 */
class SPRMCommand : private CommandBase
{
    private:

        /** class constants */
        enum
        {
            /** parsing receiver buffer as array of integer: positions */
            SPRM_COMMAND_POSITION, //
            SPRM_LENGTH_POSITION, //
            SPRM_PARAMETER_ID_POSITION, //
            SPRM_PARAMETER_VALUE_POSITION
        };

        /** the command data to be sent to the sensor. */
        struct
        {
                char mCommandID[4];
                long mLength;
                long mParameterID;
                long mParameterValue;
                long mCRC;
        } mCommand;

        /* private assignment constructor to avoid misuse */
        explicit
        SPRMCommand(const SPRMCommand& src);

        /* private assignment operator to avoid misuse */
        SPRMCommand&
        operator =(const SPRMCommand& src);

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        SPRMCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~SPRMCommand();

        /**
         * Performs the SPRM command.
         *
         * @param theParameterID
         *      the parameter ID.
         * @param theParameterValue
         *      the parameter value; the actual value set is returned.
         */
        ErrorID_t
        performCommand(int32_t theParameterID, int32_t& theParameterValue);
};

#endif /* SPRMCOMMAND_HPP_ */
