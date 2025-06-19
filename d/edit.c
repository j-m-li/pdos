
#include "std.h"

#define MAX_ARG 10
struct edit {
	struct std *std;
	int state;
	int w;
	int h;
	int cw;
	int ch;
	int cx;
	int cy;
	int type;
	int index;
	int arg[MAX_ARG+1];
};

void edit__flush(void *self)
{
	struct edit *ed = self;
	ed->std->flush();
}

void edit__print(void *self, char *buf)
{
	struct edit *ed = self;
	int l = 0;
	while (buf[l]) {
		l++;
	}
	ed->std->print(buf, l);
}

void edit__printn(void *self, int v)
{
	char buf[50];
	int i = 0;
	int m;
	m = 1000000000;
	if (v < 0) {
		buf[0] = '-';
		i++;
		v = -v;
	}
	if (v < 0) {
		v = -(v+1);
		buf[i] = (int)'0' + (v / m);
		i++;
		v = (v % m) - 1;
		m = m / 10;
	} else {
		while (m > 1 && m > v) {
			m = m / 10;
		}
	}
	while (m > 0) {
		buf[i] = (int)'0' + (v / m);
		i++;
		v = v % m;
		m = m / 10;

	}
	buf[i] = '\0';
	edit__print(self, buf);
}

void *edit__init(struct std *std)
{
	static struct edit ed;
	ed.std = std;
	ed.state = 0;
  	ed.cw = 8;
	ed.ch = 12;
	ed.w = 80;
	ed.h = 25;
	ed.cx = 0;
	ed.cy = 0;
	edit__print(&ed, "\033c\033[2J\033[9999;9999H\033[6n\n");
	return &ed;
}

void edit__esc(void *self)
{
	struct edit *ed = self;
	ed->std->exit(0);
}

void edit__move(void *self, int dx, int dy)
{
	struct edit *ed = self;
	if (ed->index > 1 && ed->arg[0] == 1 && ed->arg[1] == 5) {
		/* FIXME ctrl pressed */
	}
	ed->cx += dx;
	ed->cy += dy;
	if (ed->cx < 0) {
		ed->cx = 0;
	} else if (ed->cx >= ed->w) {
		ed->cx = ed->w - 1;
	}
	if (ed->cy < 0) {
		ed->cy = 0;
	} else if (ed->cy >= ed->h) {
		ed->cy = ed->h - 1;
	}
	edit__print(self, "\033[");
	edit__printn(self, ed->cy + 1);
	edit__print(self, ";");
	edit__printn(self, ed->cx + 1);
	edit__print(self, "H");
}

void edit__enter(void *self)
{
	struct edit *ed = self;
	ed->cx = 0;
	edit__move(self, 0, 1);

}

void edit__backspace(void *self)
{
	edit__move(self, -1,0);
}

void edit__alt(void *self, int key)
{
	char buf[4];
	buf[0] = '^';
	buf[1] = key;
	buf[2] = 0;
	edit__print(self, buf);
}

void edit__special(void *self)
{
	struct edit *ed = self;
	if (ed->index < 0) {
		return;
	}
	switch (ed->arg[0]) {
	case 1: /* home */
	case 2: /* insert */
		break;
	case 3:
		/*FIXME foward delete*/
		edit__print(self, " ");
		edit__move(self, -1,0);
		break;
	case 4: /* end */
	case 5: /* page up */
	case 6: /* page down  */
		if (ed->index > 0 && ed->arg[1] == 5) {
			/* ctrl pressed */
		}
	}
}

void edit__machine(void *self, char *buf, int l)
{
	struct edit *ed = self;
	if (l < 1) {
		return;
	}
	switch (ed->state) {
	case 0:
		edit__print(self, "\033[0;0H");
		ed->state = 1;
		edit__machine(self, buf, l);
		return;
	case 1:
		if (buf[0] == '\033') {
			ed->state = 2;
			edit__machine(self, buf + 1, l - 1);
			return;
		} else if (buf[0] == '\b' || buf[0] == 0x7F) {
			edit__backspace(self);
		} else if (buf[0] == '\n') {
			edit__enter(self);
		} else {
			ed->std->print(buf, 1);
		}
		if (l > 1) {
			edit__machine(self, buf + 1, l - 1);
		}
		return;
		break;
	case 2:
		switch (buf[0]) {
		case '\033':
			edit__esc(self);
			ed->state = 1;
			break;
		case '[':
			ed->type = '[';	
			ed->state = 3;
			ed->index = -1;
			ed->arg[0] = 0;
			ed->arg[1] = 0;
			break;
		default:
			if (buf[0] >= 'a' && buf[0] <= 'z') {
				edit__alt(self, buf[0]);
			}
			ed->state = 1;
		}
		edit__machine(self, buf + 1, l - 1);
		return;
	case 3:
		if (buf[0] >= '0' && buf[0] <= '9') {
			if (ed->index < 0) {
				ed->index = 0;
			}
			ed->arg[ed->index] = ed->arg[ed->index] * 10 + 
				buf[0] - '0';
		} else if (buf[0] == ';') {
			if (ed->index < MAX_ARG) {
				ed->index++;
				ed->arg[ed->index] = 0;
			}
		} else {
			switch (buf[0]) {
			case 'A':
				edit__move(self,0,-1);
				break;
			case 'B':
				edit__move(self,0,1);
				break;
			case 'C':
				edit__move(self,1,0);
				break;
			case 'D':
				edit__move(self,-1,0);
				break;
			case '~':
				edit__special(self);
				break;
			}
			ed->index = -1;
			ed->state = 1;
		}
		edit__machine(self, buf + 1, l - 1);
		return;
	default:
		ed->state = 1;
	}
}

int edit__event(void *self, char *buf, int l)
{
	struct edit *ed = self;
	edit__machine(self, buf, l);
	edit__flush(self);
	return 0;
}

int edit__idle(void *self)
{
	return 0;
}

