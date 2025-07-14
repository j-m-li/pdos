

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <debugapi.h>
#pragma comment(lib, "user32.lib")
#elif defined(__wasm32__)

#else

#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#ifdef C90
#else
#include <termios.h>
#include <sys/select.h>
#endif

#include <sys/ioctl.h>
#include <unistd.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "tk.h"

extern int edit__event(void *self, char *buf, int l);
extern int edit__idle(void *self);
extern void *edit__init(struct tk *tk, void *win);

#ifdef _WIN32

static HANDLE hStdin;
static DWORD fdwSaveOldMode;

int term__deinit()
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hIn, fdwSaveOldMode);
       	return 0;
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        Beep(750, 300);
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        Beep(600, 200);
        printf("Ctrl-Close event\n\n");
        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        Beep(900, 200);
        printf("Ctrl-Break event\n\n");
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        Beep(1000, 200);
        printf("Ctrl-Logoff event\n\n");
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        Beep(750, 500);
        printf("Ctrl-Shutdown event\n\n");
        return FALSE;

    default:
        return FALSE;
    }
}

VOID ErrorExit (LPSTR lpszMessage)
{
    fprintf(stderr, "%s\n", lpszMessage);

    // Restore input mode on exit.

    SetConsoleMode(hStdin, fdwSaveOldMode);

    ExitProcess(0);
}

// https://learn.microsoft.com/fr-fr/windows/win32/inputdev/virtual-key-codes

VOID KeyEventProc(KEY_EVENT_RECORD ker, char *buf, DWORD max,  DWORD *ret)
{
    DWORD i;

    if (ker.dwControlKeyState & CAPSLOCK_ON )
    {

    }
    if (ker.dwControlKeyState & ENHANCED_KEY )
    {
        
    }
    if (ker.dwControlKeyState & LEFT_ALT_PRESSED )
    {
        
    }
    if (ker.dwControlKeyState & LEFT_CTRL_PRESSED )
    {
        if (ker.wVirtualKeyCode == 'C') {
            CtrlHandler(CTRL_C_EVENT);
        }   
    }
    if (ker.dwControlKeyState & NUMLOCK_ON )
    {
        
    }
    if (ker.dwControlKeyState & RIGHT_ALT_PRESSED )
    {
        
    }
    if (ker.dwControlKeyState & RIGHT_CTRL_PRESSED )
    {
        
    }
    if (ker.dwControlKeyState & SCROLLLOCK_ON )
    {
        
    }
    if (ker.dwControlKeyState & SHIFT_PRESSED )
    {
        
    }
    if(ker.bKeyDown) {
        for (i = 0; i < ker.wRepeatCount && (*ret) < (max-1); i++) {
            buf[*ret] = ker.uChar.AsciiChar;
            *ret = *ret + 1;
        }
        switch (ker.wVirtualKeyCode) {
        case VK_BACK:
        case VK_TAB:
        // TODO
            break;
        }
    }
    else  {
        //printf("key released\n");
    }
}

// https://learn.microsoft.com/en-us/windows/console/mouse-event-record-str

VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
    printf("Mouse event: ");

    switch(mer.dwEventFlags)
    {
        case 0:

            if(mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                // x an y start from 0
                printf("left button press %d %d\n", mer.dwMousePosition.X, mer.dwMousePosition.Y);
            }
            else if(mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
            {
                printf("right button press \n");
            }
            else if (mer.dwButtonState == 0) {
                    printf("button release\n");
            } else
            {
                printf("button press\n");
            }
            break;
        case DOUBLE_CLICK:
            printf("double click\n");
            break;
        case MOUSE_HWHEELED:
            printf("horizontal mouse wheel\n");
            break;
        case MOUSE_MOVED:
            printf("mouse moved %d %d\n", mer.dwMousePosition.X, mer.dwMousePosition.Y);
            break;
        case MOUSE_WHEELED:
            printf("vertical mouse wheel\n");
            break;
        default:
            printf("unknown\n");
            break;
    }
}

VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    printf("Resize event\n");
    printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
}


int getkey(char *buffer, int *len)
{
    DWORD rd;
    DWORD cNumRead,i;
    INPUT_RECORD irInBuf[128];
    int timeout = 0;
    rd = 0;
    cNumRead = 0;
    if (1) {
        rd = 0;
        if (WaitForSingleObject(hStdin, 100)) {
            return 0;
        }
        if (! ReadConsoleInputA(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead) ) // number of records read
            ErrorExit("ReadConsoleInput");
        for (i = 0; i < cNumRead; i++)
        {
            switch(irInBuf[i].EventType)
            {
                case KEY_EVENT: // keyboard input
                    KeyEventProc(irInBuf[i].Event.KeyEvent, buffer, *len, &rd);
                    break;

                case MOUSE_EVENT: // mouse input
                    MouseEventProc(irInBuf[i].Event.MouseEvent);
                    break;

                case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
                    ResizeEventProc( irInBuf[i].Event.WindowBufferSizeEvent );
                    break;

                case FOCUS_EVENT:  // disregard focus events

                case MENU_EVENT:   // disregard menu events
		    buffer[0] = 0;
                    rd = 1;
                    break;

                default:
                    ErrorExit("Unknown event type");
                    break;
            } 
        }
    }
    fflush(stdout);
    if (rd > 0) {
	*len = rd;
    } else {
	*len = 0;
    }        
    return 1;
}

void __builtin_trap(void)
{
	DebugBreak();
}

int term__init(int a)
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
       	if (hStdin == INVALID_HANDLE_VALUE) {
	           return -1;
        }

        if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
	            return -1;
        }
	//dwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS /*   | ENABLE_PROCESSED_INPUT| ENABLE_VIRTUAL_TERMINAL_PROCESSING  */| ENABLE_VIRTUAL_TERMINAL_INPUT;
	//dwMode &= ~(ENABLE_PROCESSED_INPUT);
	if (!SetConsoleMode(hStdin, ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) 
	{
		return -1;
	}
	atexit((void(*)(void))(*term__deinit));

    	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        	return -1;
	}
     	/* UTF-8 */
     	SetConsoleOutputCP(65001); //chcp 65001
 	SetConsoleCP(65001);
	return 0;
}

int clipboard__set(char *txt, int len) 
{ 
    wchar_t  *lptstrCopy; 
    static HGLOBAL hglbCopy = 0; 
    int unilen;
 
    if (!OpenClipboard(0)) 
        return -1; 
    unilen = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (void*)txt,
        len,
        (void*)0,
        0 
        );
    if (unilen < 1) { 
        CloseClipboard(); 
        return -2; 
    }  
    EmptyClipboard(); 
    if (hglbCopy) {
        GlobalFree(hglbCopy);
    }
    hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * 4); 
    if (hglbCopy == NULL) { 
        CloseClipboard(); 
        return -2; 
    } 
    lptstrCopy = GlobalLock(hglbCopy); 
    
    if (!MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (void*)txt,
        len,
        lptstrCopy,
        unilen
        )
        )
    {
        CloseClipboard(); 
        return -1; 
    }
    lptstrCopy[unilen] = 0;
    GlobalUnlock(hglbCopy); 
    SetClipboardData(CF_UNICODETEXT, hglbCopy);   
    CloseClipboard(); 
    return 0; 
}


char  *clipboard__get()
{
    static char *buffer = NULL;
    wchar_t *unistr;
    HANDLE clip;
    int len;

    
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    if (!OpenClipboard(0)) 
        return ""; 
        
    clip = GetClipboardData(CF_UNICODETEXT);
    if (!clip) {
         CloseClipboard();
         return ""; 
    }
    
    unistr = (wchar_t*)GlobalLock(clip);
    if (!unistr) {
         CloseClipboard();
         return ""; 
    }
    
    len = WideCharToMultiByte(CP_UTF8, 0, unistr, -1, NULL,  0, NULL, NULL);
    
    buffer = malloc(len + 2);
    
    if (buffer && (len > 0)) {
        len = WideCharToMultiByte(CP_UTF8, 0, unistr, -1, buffer,  len + 1, NULL, NULL);
        buffer[len] = 0;
    } else if (buffer) {
        buffer[0] = 0;
    }
    GlobalUnlock(clip);
    CloseClipboard();
    return buffer;
}

#elif defined(C90)

int term__init(int a)
{
	system("stty -icanon -echo -isig -iexten iutf8");
	return 0;
}

int _kbhit()
{
	return 1;
}

int _getch()
{
	return getchar();
}

#else
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
	tv.tv_usec = 1000 * 20;
    	if (select(1, &fds, NULL, NULL, &tv) != 0) {
		return 1;
	}
	return 0;

}

#endif

void flush()
{
	fflush(stdout);
}

int print(char *buf, int l)
{
	int i;
	if (!buf) return -1;
	for (i = 0; i < l; i++) {
		if (buf[i] == '\n') {
			__builtin_trap();
		} else if (buf[i] == '\r') {
			__builtin_trap();
		} else if (buf[i] == '\t') {
			__builtin_trap();
		}
	}
	fwrite(buf, 1, l, stdout);
	/*
	int i;
	for (i = 0; i < l; i++) {
		putchar(buf[i]);
	}*/
	return 0;
}

void *alloc(int size)
{
	if (size < 1 || size > 0x10000000) {
		return (void*)0;
	}
	return malloc(size);
}

void freemem(void *m)
{
	free(m);
}

void exitnow(char *txt)
{
	printf("\x1B[?1000l\x1B[?1003l\x1B[?1015l\x1B[?1006l"); 
	printf("\x1B[r"); 
	printf("\x1B[?1049l"); 
	fflush(stdout);
	fflush(stderr);
	fflush(stdin);
	printf("%s\n", txt);
#ifndef _WIN32
	system("stty sane");
#endif
	exit(10);
}

static void *win;
static void *tk;
static void *edit;
static struct std std;

void term_size(int *w, int *h)
{

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else 
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	*w = win.ws_col;
	*h = win.ws_row;
#endif
}

void setup(void)
{
	char *txt;
	setvbuf(stdin, NULL, _IONBF, 0);

	term__init(0);

	std.print = print;
	std.flush = flush;
	std.exit = exitnow;
	std.alloc = alloc;
	std.free = freemem;

	tk = tk__init(&std);
	if (!tk) {
		exit(1);
	}
	win = tk_block(tk, 0, 0, TK_FULL, TK_FULL, (void*)0);
	if (!win) {
		exit(2);
	}
	txt = "hello world\nmix\ttop\n\tok\n\t";
	tk_block__add_text(tk, win, txt, strlen(txt), (void*)0); 
	edit = edit__init(tk, win);
	fflush(stdout);

	edit__idle(edit);
}

void loop(void)
{
	char buf[32];
	int l;
#if defined(C90)
	int c;
#endif
		
#if defined(_WIN32) 
		l = sizeof(buf);
		if (getkey(buf , &l)) {
			
#elif defined(C90)
		if (_kbhit()) {
			c = -1;
			l = 0;
			while (c < 1 || c > 0xFF) {
				c = _getch();
			}
			buf[0] = c;
			l = 1;
			

#else
		if (_kbhit()) {
			l = fread(buf,1,1,stdin);
			
			if (ferror(stdin)) {
				exit(-2);
			}
/*
			if (l == 1 && buf[0] == '\033') {
				l = 2;
				buf[1] = '\033';
			}
*/
#endif

#if 0
			buf[l] = 0;
			printf(" %x %d", buf[0], l, buf[1], buf[2], buf[3]);		
			flush(stdout);
			return;
#endif

			if (l > 0) {
				edit__event(edit, buf, l);
				edit__idle(edit);
			}
		}

}

int main(int argc, char *argv[])
{
	setup();
	while (1) {
		loop();
	}
	return 0;
}

