/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GPRMCommand.hpp - Reading sensor parameters
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

#ifndef GPRMCOMMAND_HPP_
#define GPRMCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class GPRMCommand.hpp - Reading a parameter from the sensor.
 */
class GPRMCommand : private CommandBase
{
    private:
        /** class constants */
        enum
        {
            // parsing receiver buffer as array of integer: positions
            GPRM_COMMAND_POSITION, //
            GPRM_LENGTH_POSITION, //
            GPRM_PARAMETER_ID_POSITION, //
            GPRM_PARAMETER_VALUE_POSITION
        };

        /** the command data to be sent to the sensor. */
        struct
        {
                char mCommandID[4];
                long mLength;
                long mParameterID;
                long mCRC;
        } mCommand;

        /* private assignment constructor to avoid misuse */
        explicit
        GPRMCommand(const GPRMCommand& src);

        /* private assignment operator to avoid misuse */
        GPRMCommand&
        operator =(const GPRMCommand& src);

        /*
         * Overwrite: Performs the GPRM command.
         */
        inline ErrorID_t
        performCommand()
        {
            return ERR_UNSUPPORTED_FUNCTION;
        }

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        GPRMCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~GPRMCommand();

        /**
         * Performs the GPRM command.
         *
         * @param theParameterID
         *      the parameter ID.
         * @param theParameterValue
         *      the parameter value; the actual value set is returned.
         */
        ErrorID_t
        performCommand(int32_t theParameterID, int32_t& theParameterValue);
};

#endif /* GPRMCOMMAND_HPP_ */
