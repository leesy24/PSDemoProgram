/*
 * KbhitGetch.h
 *
 *  Created on: 2016. 10. 13.
 *      Author: leesy
 */

#ifndef SRC_KBHITGETCH_H_
#define SRC_KBHITGETCH_H_
	#if __WIN32__
		/* On windows, we can use kbhit() to check if ESC has been pressed. */
		#include <conio.h>

		#define changemode(dir)
	#elif __linux__
		#if defined (__cplusplus)
			extern "C" {
		#endif

			void changemode(int dir);
			int kbhit (void);
			int getch(void);

		#if defined (__cplusplus)
			}
		#endif
	#else
		#error "This program requires Linux or Win32."
	#endif
#endif /* SRC_KBHITGETCH_H_ */
