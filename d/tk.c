
#include "tk.h"

void tk__flush(void *self)
{
	struct tk *ed = self;
	ed->std->flush();
}

void tk__print(void *self, char *buf)
{
	struct tk *ed = self;
	int l = 0;
	while (buf[l]) {
		l++;
	}
	ed->std->print(buf, l);
}

void tk__printn(void *self, int v)
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
	tk__print(self, buf);
}

int tk__measure_string(struct tk *tk, char *txt, int len, 
		int *w, int *h, int *a)
{
	int i;
	*h = 1;
	*a = 1;
	*w = 0;
	for (i = 0; i < len; i++) { /* UTF-8 */
		if (txt[i] == '\r' && txt[i+1] == '\n') {
			i += 2;
			break;
		} else if (txt[i] == '\n') {
			i++;
			break;
		}
		if ((txt[i] & 0xFF) <= 0x7F) {
			if (txt[i] == '\t') {

			}
			*w += 1;
		} else if (((int)txt[i] & 0xFF) >= 0xC0) { 
			*w += 1;
		}
	}
	return i;
}

int tk__draw_string(struct tk *tk, char *txt, int len)
{
	int w,h,a;
	tk__measure_string(tk, txt, len, &w, &h, &a);
	tk->std->print(txt, len);
	return w;
}

void tk__clear_screen(struct tk *tk)
{
	tk__print(tk, "\033[3J");
}

void tk__move_to(struct tk *tk, int x, int y)
{
	tk__print(tk, "\033[");
	tk__printn(tk, y + 1);
	tk__print(tk, ";");
	tk__printn(tk, x + 1);
	tk__print(tk, "H");
}
#include <stdio.h>
void tk__clear_rect(struct tk *tk, int x, int y, int w, int h )
{
	int i, l, k;
/*	
	if (x == 0 && (w == 0 || h == 0)) {
		tk__move_to(tk, x, y);
		tk->std->print(" ", 1);
	}*/

	for (i = y; h > 0; i++,h--) {
		tk__move_to(tk, x, i);
		l = 10;
		for (k = w; k > 0; k -= 10) {
			if (k < 10) {
				l = k;
			}
			tk->std->print("          ", l);
		}
	}
	tk__move_to(tk, 0, 0);
}


void tk__print_status(struct tk *tk, char *txt, int a, int b)
{
	tk__move_to(tk, 0, tk->h - 1);
	tk__print(tk, txt);
	tk__print(tk, "  ");
	tk__printn(tk, a);
	tk__print(tk, ";");
	tk__printn(tk, b);
	tk__print(tk, "  ");
}

void tk__hide_cursor(struct tk *tk)
{
	tk__print(tk, "\033[?25l");
}
void tk__show_cursor(struct tk *tk, int x, int y)
{
	tk->cursor_x = x;
	tk->cursor_y = y;
	tk__move_to(tk, x, y);
	tk__print(tk, "\033[?25h");
}
void tk__set_rev(struct tk *tk)
{
	tk__print(tk, "\033[7m");
}
void tk__clr_rev(struct tk *tk)
{
	tk__print(tk, "\033[27m");
}

void *tk__init(struct std *std)
{
	struct tk *tk;
	if (!std) {
		return std;
	}
	tk = std->alloc(sizeof(struct tk));
	if (!tk) {
		return tk;
	}
	tk->std = std;
	tk->w = 80;
	tk->h = 24;
	tk->cursor_x = 0;
	tk->cursor_y = 0;
	tk->draw_string = tk__draw_string;
	tk->measure_string = tk__measure_string;
	tk->move_to = tk__move_to;
	tk->clear_rect = tk__clear_rect;
	tk->print_status = tk__print_status;
	tk->show_cursor = tk__show_cursor;
	tk->hide_cursor = tk__hide_cursor;
	tk->set_rev = tk__set_rev;
	tk->clr_rev = tk__clr_rev;
	return tk;	
}

void *tk__create(struct tk *tk, int type)
{
	void *m;
	switch (type) {
	case TK_TEXT:
		m = tk->std->alloc(sizeof(struct tk_text));
		break;
	case TK_INLINE:
		m = tk->std->alloc(sizeof(struct tk_inline));
		break;
	case TK_RANGE:
		m = tk->std->alloc(sizeof(struct tk_range));
		break;
	case TK_BLOCK:
		m = tk->std->alloc(sizeof(struct tk_block));
		break;
	default:
		tk->std->exit("unknow widget");
	}
	if (!m) {
		return m;
	}
	((struct tk_text *)m)->display = type;
	return m;
}

