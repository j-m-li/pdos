
#include "tk.h"

#define MAX_ARG 10
struct edit {
	struct std *std;
	struct tk *tk;
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


void *edit__init(struct tk *tk, void *win)
{
	static struct edit ed;
	ed.std = tk->std;
	ed.tk = tk;
	ed.tk->root = win;
	ed.state = 0;
  	ed.cw = 8;
	ed.ch = 12;
	ed.w = 80;
	ed.h = 25;
	ed.cx = 0;
	ed.cy = 0;
	edit__print(&ed, "\033c\033[2J\033[9999;9999H\033[6n");
	edit__print(&ed, "\033[!p\033[?1049h\033[?1003h\033[1015h\033?1006h");
	edit__flush(&ed);
	return &ed;
}

void edit__esc(void *self)
{
	struct edit *ed = self;
	ed->std->exit("");
}

void edit__move(void *self, int dx, int dy, int shift)
{
	struct edit *ed = self;
	if (shift) {
		tk_block__move_cursor(ed->tk, ed->tk->root, dx, dy, TK_FLAG_END);
	} else {
		tk_block__move_cursor(ed->tk, ed->tk->root, 
				dx, dy, TK_FLAG_COLLAPSE);
	}
}

void edit__tab(void *self)
{
	struct edit *ed = self;
	struct tk_block *win = ed->tk->root;
	tk_block__add_text(ed->tk, win, "\t", 1, win->selection);
}

void edit__enter(void *self)
{
	struct edit *ed = self;
	struct tk_block *win = ed->tk->root;
	ed->cx = 0;
	tk_block__add_text(ed->tk, win, "\n", 1, win->selection);

}

void edit__del(void *self)
{
	struct edit *ed = self;
	struct tk_block *win = ed->tk->root;
	tk_block__move_cursor(ed->tk, win, 1, 0, TK_FLAG_END);
	tk_block__add_text(ed->tk, win, "", 0, win->selection);
}

void edit__backspace(void *self)
{
	struct edit *ed = self;
	struct tk_block *win = ed->tk->root;
	tk_block__move_cursor(ed->tk, win, -1, 0, TK_FLAG_START);
	tk_block__add_text(ed->tk, win, "", 0, win->selection);
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
		/* foward delete*/
		edit__del(self);
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
	int n = 1;
	int shift = 0;
	int ctrl = 0;
	struct edit *ed = self;
	struct tk_block *win = ed->tk->root;
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
		} else if (buf[0] == '\t') {
			edit__tab(self);
		} else if (buf[0] == '\n' || buf[0] == '\r') {
			edit__enter(self);
		} else {
			while (n < l) {
				if (buf[n] == '\033') {
					break;
				}
				n++;
			}
			tk_block__add_text(ed->tk, 
					win, buf, n, win->selection);
		}
		l -= n;
		if (l > 1) {
			edit__machine(self, buf + n, l);
		}
		return;
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
			ed->tk->print_status(ed->tk, "JML ", ed->index, ed->arg[0]);
			if (ed->index >= 0 && ed->arg[ed->index] == 2) {
				shift = 1;
			}
			if (ed->index >= 0 &&  ed->arg[ed->index] == 5) {
				ctrl = 1;
			}
			switch (buf[0]) {
			case 'A':
				edit__move(self,0,-1,shift);
				break;
			case 'B':
				edit__move(self,0,1,shift);
				break;
			case 'C':
				edit__move(self,1,0,shift);
				break;
			case 'D':
				edit__move(self,-1,0,shift);
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
	struct edit *ed = self;

	ed->tk->hide_cursor(ed->tk);
	ed->tk->pos.flags = TK_FLAG_DIRTY;
	ed->tk->pos.x = 0;
	ed->tk->pos.y = 0;
	ed->tk->pos.col = 0;
	tk_block__draw(ed->tk, ed->tk->root, &ed->tk->pos);
	ed->tk->show_cursor(ed->tk, ed->tk->cursor_x, ed->tk->cursor_y);
	ed->std->flush();
	return 0;
}

