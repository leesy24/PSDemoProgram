/*
 * WinClientUART.hpp
 *
 *  Created on: 2016. 10. 21.
 *      Author: leesy
 */

#ifndef SRC_WINCLIENTUART_HPP_
#define SRC_WINCLIENTUART_HPP_

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "IDataStream.hpp"

class ClientUART : public IDataStream
{
	private:
		/** a flag indicating an opened socket. */
		bool mIsOpen;

        /** the sensors UART port */
		string32_t mPort;

		HANDLE hComm;                          // Handle to the Serial port

		/** Pointer at a log file to store received data. */
		FILE *mLogFile;

		/** timeout in ms. */
		int mTimeout;

        /** private assignment constructor to avoid misuse */
        explicit
		ClientUART(const ClientUART& src);

        /** private assignment operator to avoid misuse */
        ClientUART&
        operator =(const ClientUART& src);

	public:
		ClientUART();

		virtual ~ClientUART();

		ErrorID_t config(const char* thePort, int32_t theTimeout, FILE* theLogFile);

		virtual ErrorID_t open();

		inline bool isOpen()
        {
            return mIsOpen;
        }

		ErrorID_t close();

		int32_t read(void* buffer, int32_t size);

		int32_t write(void* buffer, int32_t size);
};

#endif /* SRC_WINCLIENTUART_HPP_ */
