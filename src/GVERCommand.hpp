/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       GVERCommand.hpp - Reading the firmware version.
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/11/30 08:23:47 $
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
 * 09.11.2015 CBD:
 * - Changed: clear version string if GVER was not successful.
 */

#ifndef GVERCOMMAND_HPP_
#define GVERCOMMAND_HPP_

#include "CommandBase.hpp"

/**
 * Class GVERCommand.hpp - Reading the firmware version.
 *
 * <p>
 * <b>Executive summary:</b>
 * <p>
 * Requests the firmware version from a sensor connected.
 *  <p>&nbsp;<p>
 *
 *
 * <b>OS/Hardware Dependencies</b>
 * <p>
 * Requires the winsock library, however, the code can easily be ported
 * to any other device.
 * <p>&nbsp;<p>
 */
class GVERCommand : private CommandBase
{
    private:

        /** the command data to be sent to the sensor. */
        struct
        {
                char mCommandID[4];
                long mLength;
                long mCRC;
        } mCommand;

        /** the firmware version */
        char mVersion[1024];

        /* private assignment constructor to avoid misuse */
        explicit
        GVERCommand(const GVERCommand& src);

        /* private assignment operator to avoid misuse */
        GVERCommand&
        operator =(const GVERCommand& src);

    public:
        /**
         * Constructor.
         *
         * @param theDataStream
         *      the connection to be used; must be open.
         */
        GVERCommand(IDataStream& theDataStream);

        /**
         * Destructor.
         */
        virtual
        ~GVERCommand();

        /**
         * Return the version string received.
         *
         * @return
         *          the firmware version string.
         */
        inline const char*
        getVersion()
        {
            return mVersion;
        }

        /**
         * Performs the GVER command.
         *
         * @return
         *      ERR_SUCCESS on success, otherwise a negative error code.
         */
        ErrorID_t
        performCommand();
};

#endif /* GVERCOMMAND_HPP_ */
