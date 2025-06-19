

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

#include "std.h"

extern int edit__event(void *self, char *buf, int l);
extern int edit__idle(void *self);
extern void *edit__init(struct std *std);

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

void flush()
{
	fflush(stdout);
}

int print(char *buf, int l)
{
	fwrite(buf, 1, l, stdout);
	/*
	int i;
	for (i = 0; i < l; i++) {
		putchar(buf[i]);
	}*/
	return 0;
}

int main(int argc, char *argv[])
{
	char buf[32];
	int l;
	int c;
	void *edit;
	struct std std;

	setvbuf(stdin, NULL, _IONBF, 0);
#ifdef __WIN32__
#else
	term__init(0);
#endif
	std.print = print;
	std.flush = flush;
	std.exit = exit;
	edit = edit__init(&std);

	while (1) {
		if (_kbhit()) {
#ifdef __WIN32__
			l = 0;
			while (l < 1) {
				c = getchar();
				if (c < 1) {
					break;
				}
				buf[l] = c;
				l++;
			}
#else
			l = fread(buf, 1, sizeof(buf) - 1, stdin);
			if (ferror(stdin)) {
				exit(0);
			}
			if (l == 1 && buf[0] == '\033') {
				l = 2;
				buf[1] = '\033';
			}
#endif
			buf[l] = 0;
			if (l > 0) {
				edit__event(edit, buf, l);
			}
		}
		edit__idle(edit);
	}
	return 0;
}

