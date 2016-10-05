/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       SCANCommand.hpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/11/30 08:24:53 $
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
 * 24.11.2015 CBD:
 * - Changed: setParameter() to set a SCAN command parameter.
 */

#ifndef SCANCOMMAND_HPP_
#define SCANCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class SCANCommand.hpp - send SCAN to the scanner.
 *
 * <p>
 * <b>Executive summary:</b>
 * <p>
 *  <p>&nbsp;<p>
 *
 *
 * <b>OS/Hardware Dependencies</b>
 * <p>
 * Requires the winsock library, however, the code can easily be ported
 * to any other device.
 * <p>&nbsp;<p>
 */

class SCANCommand : private CommandBase
{
    private:

        /** the command data to be sent to the sensor. */
        struct
        {
                char mCommandID[4]; /// 4 byte command ID SCAN
                long mLength; /// length of the following payload
                long mParameter; /// the SCAN parameter
                long mCRC; /// the CRC checksum
        } mCommand;

        /* private assignment constructor to avoid misuse */
        explicit
        SCANCommand(const SCANCommand& src);

        /* private assignment operator to avoid misuse */
        SCANCommand&
        operator =(const SCANCommand& src);

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        SCANCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~SCANCommand();

        /**
         * Performs the SCAN command.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        ErrorID_t
        performCommand();

        /**
         * Set a parameter for SCAN.
         *
         * @param theParameter
         *      the parameter value to be set.
         */
        void setParameter(int32_t theParameter);
};

#endif /* SCANCOMMAND_HPP_ */
