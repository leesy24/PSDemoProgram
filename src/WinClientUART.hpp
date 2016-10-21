/*
 * ClientCommunication.hpp
 *
 *  Created on: 2016. 10. 21.
 *      Author: leesy
 */

#ifndef SRC_WINCLIENTUART_HPP_
#define SRC_WINCLIENTUART_HPP_

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "ErrorID.h"

class ClientUART
{
	private:
		/** a flag indicating an opened socket. */
		bool mIsOpen;

		HANDLE hComm;                          // Handle to the Serial port

		/** Pointer at a log file to store received data. */
		FILE *mLogFile;

		/** timeout in ms. */
		int mTimeout;

	public:
		ClientUART();

		~ClientUART();

		ErrorID_t config(int32_t theTimeout, FILE* theLogFile);

		ErrorID_t open(const char* thePort);

		ErrorID_t close();

		int32_t read(void* buffer, int32_t size);

		int32_t write(void* buffer, int32_t size);
};

#endif /* SRC_WINCLIENTUART_HPP_ */
