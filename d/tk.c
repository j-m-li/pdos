
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

void tk__measure_string(struct tk *tk, char *txt, int len, 
		int *w, int *h, int *a)
{
	int i;
	*h = 1;
	*a = 1;
	*w = 0;
	for (i = 0; i < len; i++) { /* UTF-8 */
		if (txt[i] <= 0x7F) {
			*w += 1;
		} else if (((int)txt[i] & 0xFF) >= 0xC0) { 
			*w += 1;
		}
	}
}

void tk__draw_string(struct tk *tk, char *txt, int len)
{
	tk->std->print(txt, len);
}

void tk__move_to(struct tk *tk, int x, int y)
{
	tk__print(tk, "\033[");
	tk__printn(tk, y + 1);
	tk__print(tk, ";");
	tk__printn(tk, x + 1);
	tk__print(tk, "H");
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
	tk->h = 25;
	tk->draw_string = tk__draw_string;
	tk->measure_string = tk__measure_string;
	tk->move_to = tk__move_to;
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

