/*
 * KbhitGetch.c
 *
 *  Created on: 2016. 10. 13.
 *      Author: leesy
 */

#if defined(__linux__) || (defined(__CYGWIN__) && !defined(_WIN32))
/* Ref. http://cboard.cprogramming.com/linux-programming/51531-faq-cached-input-mygetch.html?highlight=kbhit */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

void changemode(int dir)
{
  static struct termios oldt, newt;

  if ( dir == 1 )
  {
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit (void)
{
  struct timeval tv;
  fd_set rdfs;
  int ret;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);

  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  ret = FD_ISSET(STDIN_FILENO, &rdfs);

  //fprintf(stderr, "FD_ISSET return %d\r\n", ret);

  return ret;
}

int getch(void)
{
	return getchar();
}
#endif
