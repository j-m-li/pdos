

#ifdef __WIN32__
#include <conio.h>
#else
#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <unistd.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef __WIN32__
struct termios orig_termios = {0};
struct sigaction old_action;

void sigint_handler(int sig_no)
{
	/*
	term__deinit();
    	sigaction(SIGINT, &old_action, NULL);
    	kill(0, SIGINT);*/
}

int term__deinit()
{
	printf("\x1B[?1000l\x1B[?1003l\x1B[?1015l\x1B[?1006l"); 
	printf("\x1B[r"); 
	printf("\x1B[?1049l"); 
	fflush(stdout);
	fflush(stderr);
	fflush(stdin);
	usleep(1000);
	tcsetattr(0, TCSANOW, &orig_termios);
	return 0;
}

int term__init(int a)
{
  	struct termios new_termios;
	struct sigaction action;

	memset(&action, 0, sizeof(action));
	action.sa_handler = &sigint_handler;
    	sigaction(SIGINT, &action, &old_action);
	tcgetattr(0, &orig_termios);
    	memcpy(&new_termios, &orig_termios, sizeof(new_termios));
	atexit((void(*)())(*term__deinit));
	new_termios.c_lflag &= ~(ICANON|ECHO);
	new_termios.c_cc[VMIN] = 0;
	new_termios.c_cc[VTIME] = 0;
    	tcsetattr(0, TCSAFLUSH, &new_termios);
	fflush(stdout);
	return 0;
}

int _kbhit()
{
	struct timeval tv = { 0L, 0L };
    	fd_set fds;
    	FD_ZERO(&fds);
    	FD_SET(0, &fds);
	tv.tv_usec = 1000 * 10;
    	if (select(1, &fds, NULL, NULL, &tv) != 0) {
		return 1;
	}
	return 0;

}
static int _getch()
{
	return getchar();
}


#endif

int main(int argc, char *argv[])
{
	int c;

#ifdef __WIN32__
	setvbuf(stdin, NULL, _IONBF, 0);
#else
	term__init(0);
#endif
	printf("\033[32;44mEdit\033[39;49m\n");
	while (1) {
		if (_kbhit()) {
			c = _getch();
			if (c == 'q' || c < 0) {
				exit(0);
			}
			printf("%c-\n", c);
		}
	}
	return 0;
}

