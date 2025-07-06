#include "tk.h"

void tk_inline__init(struct tk *tk, void *self_)
{
	struct tk_inline *self = self_;
	self->next = (void*)0;
	self->flags = 0;
}

void *tk_inline(struct tk *tk)
{
	struct tk_inline *self = tk__create(tk, TK_INLINE);
	tk_text__init(tk, self, (void*)0, 0);
	tk_inline__init(tk, self);
	return self;
}

int  tk_inline__measure_string(struct tk *tk, void *self_, int start, 
		int end, int *w, int *h, int *a, struct tk_pos *pos)
{
	struct tk_inline *self = self_;
	int s;
	int i;
	int w1,  n;
	char *c;
	int o = 0;
	*w = 0;
	if (!self->data ||  !(self->display == TK_INLINE || 
				self->display == TK_TEXT) ||
			start >= end) 
	{
		return tk->measure_string(tk, "", 0, w, h, a);
	}
	c = self->data;
	s = start;
	i = s;
	while (i < end) {
		if (c[i] == '\t') {
			if (i != s)  {
				o += tk->measure_string(tk, 
						c+s,i-s, &w1, h, a);
				*w += w1;
			}
			o++;
/*			tk->measure_string(tk, " ", 1, &w1, h, a);
				*w += w1;
*/
			tk->measure_string(tk, " ", 1, &w1, h, a);
			if (w1 > 0) {
				n = (pos->col + *w) / w1;
				n = 8 - (n % 8);
				*w += n * w1;
			} else {
				*w += 8;
			}
			s = i + 1;
		} else if (c[i] == '\n' || c[i] == '\r') {
			if (i != s)  {
				o += tk->measure_string(tk, 
						c+s,i-s, &w1, h, a);
			} else {
				tk->measure_string(tk, 
						" ",1, &w1, h, a);
				w1 = 0;
			}
			*w += w1;
			o++;
			return o;	
		}
		i++;
	}
	if (s < i) {
		o += tk->measure_string(tk, c + s, i - s, &w1, h, a);
		*w += w1;
	}
	return o;
	
}

int  tk_inline__draw(struct tk *tk, void *self_, int start, 
		int end, struct tk_pos *pos)
{
	struct tk_inline *self = self_;
	int w = 0;
	int w1, h, a, n, s;
	char *c;
	int i;
	if (!self || !tk || start >= end) {
		return 0;
	}
	if (self->data && (self->display == TK_INLINE || 
				self->display == TK_TEXT)) 
	{
		c = self->data;
		s = start;
		i = s;
		while (i < end) {
			if (c[i] == '\t') {
				if (i != s)  {
					w += tk->draw_string(tk, 
						c + s, i - s);
				}
				/*
				w += tk->draw_string(tk, "        ", 1);
				*/
				tk->measure_string(tk, 
							" ", 1, &w1, &h, &a);
				n = 8;
				if (w1 > 0) {
					n = (pos->col + w) / w1;
					n = 8 - (n % 8);
				}
				w += tk->draw_string(tk, "        ", n);
				s = i + 1;
			} else if (c[i] == '\n' || c[i] == '\r') {
				if (i != s)  {
					w += tk->draw_string(tk, 
						c + s, i - s);
				} else {
					w += tk->draw_string(tk, 
						" ", 1);
				}
				return w;
			}
			i++;
		}
		if (s < i) {
			w += tk->draw_string(tk, c + s, i - s);
		}
	}
	return w;
}

