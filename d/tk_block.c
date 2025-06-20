
#include "tk.h"


void tk_block__init(struct tk *tk, void *self_, 
		int x, int y, int w, int h)
{
	struct tk_block *self = self_;
	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
	self->first_child = (void*)0;
	self->last_child = (void*)0;
	self->lines = (void*)0;
}

void *tk_block(struct tk *tk, int x, int y, int w, int h)
{
	struct tk_block *self = tk__create(tk, TK_BLOCK);
	tk_inline__init(tk, self);
	tk_block__init(tk, self, x, y, w, h);
	return self;
}


void tk_block__dirty_after(struct tk *tk, struct tk_block *self, 
		struct tk_inline *node, int offset)
{
	int f = 0;
	struct tk_range *r;
	r = self->lines;

	while (r) {
		if (r->start_container == node && r->start_offset >= offset) {
			f = TK_FLAG_DIRTY;
		}
		if (r->end_container == node && r->end_offset >= offset) {
			f = TK_FLAG_DIRTY;
		}
		r->flags |= f;
		r = r->next;
	}
}

static void *tk_block__get_next(struct tk *tk, void *self_, struct tk_inline *c)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	struct tk_inline *m;
	n = self->first_child;
	while (n) {
		if (n == c) {
			return (void*)1;
		}
		if (n->display != TK_INLINE && n->display != TK_TEXT) {
			m = tk_block__get_next(tk, self, c);
			if (m == (void*)1) {
				return n->next;	
			}
		}
		n = n->next;
	}
	return (void*)0;	
}

void *tk_block__next(struct tk *tk, void *self_, struct tk_inline *c)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	struct tk_inline *m;

	if (c->next) {
		return c->next;
	}
	n = self->first_child;
	while (n) {
		if (n == c) {
			return (void*)0;
		}
		if (n->display != TK_INLINE && n->display != TK_TEXT) {
			m = tk_block__get_next(tk, self, c);
			if (m == (void*)1) {
				return n->next;
			} else if (m) {
				return m;
			}
		}
		n = n->next;
	}
	return (void*)0;
}

static void *tk_block__get_prev(struct tk *tk, void *self_, struct tk_inline *c)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	struct tk_inline *m;
	struct tk_inline *p;
	n = self->first_child;
	p = (void*)0;
	while (n) {
		if (n == c) {
			if (p) {
				return p;
			}
			return (void*)1;
		}
		if (n->display != TK_INLINE && n->display != TK_TEXT) {
			m = tk_block__get_next(tk, self, c);
			if (m == (void*)1) {
				if (p) {
					return p;
				}
				return m;	
			}
		}
		n = n->next;
	}
	return (void*)0;	
}


void *tk_block__up(struct tk *tk, void *self_, struct tk_inline *c, int *o)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	return c;
}

void *tk_block__down(struct tk *tk, void *self_, struct tk_inline *c, int *o)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	return c;
}

void *tk_block__previous(struct tk *tk, void *self_, struct tk_inline *c)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	struct tk_inline *p;
	struct tk_inline *m;

	n = self->first_child;
	p = (void*)0;
	while (n) {
		if (n == c) {
			return p;
		}
		if (n->display != TK_INLINE && n->display != TK_TEXT) {
			m = tk_block__get_prev(tk, self, c);
			if (m == (void*)1) {
				return p;
			} else if (m) {
				return m;
			}
		}
		p = n;
		n = n->next;
	}
	return (void*)0;
}


void tk_block__move(struct tk *tk, void *self_, int dx, int dy, int flags)
{
	struct tk_block *self = self_;
	struct tk_inline *c = self->first_child;
	int o = 0;
	struct tk_range *r = self->selection;
	if (!r) {
		r = tk_range(tk, c, 0, c, 0);
		r->common_ancestor = self_;
		self->selection = r;
		return;	
	}
	if (!c) {
		return;
	}
	if (r->flags & TK_FLAG_REVERSE) {
		o = r->start_offset;
		c = r->start_container;
	} else {
		o = r->end_offset;
		c = r->end_container;
	}
	while (dx > 0) {
		if (o >= c->len) {
			break;
		}
		if (((char*)c->data)[o] <= 0x7F) {
			o++;
		} else {
			o++;
			while (((int)((char*)c->data)[o] & 0xFF) < 0xC0) {
				o++;
			}
		}
		dx--;
	}
	while (dx < 0) {
		if (o < 1) {
			break;
		}
		o--;
		if (((char*)c->data)[o] > 0x7F) {
			while (o > 0 && 
				((int)((char*)c->data)[o] & 0xFF) < 0xC0) 
			{
				o--;
			}
		}
		dx++;
	}
	if (dx < 0) {
		c = tk_block__previous(tk, self, c);
		if (!c) {
			return;
		}
		o = c->len;
		if (o > 0) {
			o--;
		}
	} else if (dx > 0) {
		c = tk_block__next(tk, self, c);
		if (!c) {
			return;
		}
		o = 0;
	}
	while (dy > 0) {
		c = tk_block__down(tk, self, c, &o);
		if (!c) {
			return;
		}
		dy--;
	}
	while (dy < 0) {
		c = tk_block__up(tk, self, c, &o);
		if (!c) {
			return;
		}
		dy++;
	}
	if (flags & TK_FLAG_COLLAPSE) {
		r->start_container = c;
		r->end_container = c;
		r->start_offset = o; 
		r->end_offset = o;
	} else if (r->flags & TK_FLAG_REVERSE) {
		r->start_offset = 0;
		r->start_container = c;
	} else {
		r->end_offset = 0;
		r->end_container = c;
	}
}

void *tk_block__add_text(struct tk *tk, void *self_, char *txt, int len)
{
	struct tk_block *self = self_;
	struct tk_inline *inli = (void*)0;
	if (!self->last_child) {
		inli = tk_inline(tk);
		self->last_child = inli;
		self->first_child = inli;
	} else {
		inli = self->last_child;
		if (self->lines) {
			tk_block__dirty_after(tk, self, inli, inli->len);
		}
	}
	tk_text__add(tk, inli, txt, len);
	self->flags |= TK_FLAG_DIRTY;

	return inli;
}

void tk_block__measure(struct tk *tk, void *self_, int flags)
{
	struct tk_block *self = self_;
	struct tk_range *r;
	int w, h, a;
	r = self->lines;
	while (r) {
		if (r->flags & TK_FLAG_DIRTY) {
			break;
		}
		r = r->next;
	}
	if (r && (r->start_container == self->last_child)) {
		tk->measure_string(tk, r->start_container->data + 
				r->start_offset,
				r->end_offset - r->start_offset, &w, &h, &a);
	} else {
		r = tk_range(tk, self->first_child, 0, self->last_child, 
				self->last_child->len);
		self->lines = r;
	}
	while (r) {
		r = r->next;
	}
}

void tk_block__draw_line(struct tk *tk, struct tk_block *self,
		struct tk_range *r, int flags)
{
	tk->move_to(tk, self->x, self->y);
	if (r->start_container == r->end_container) {
		tk_inline__draw(tk, r->start_container, r->start_offset,
				r->end_offset);
	} else {

	}
}

void tk_block__draw(struct tk *tk, void *self_, int flags)
{
	struct tk_block *self = self_;
	struct tk_range *r;
	if (self->flags & TK_FLAG_DIRTY) {
		tk_block__measure(tk, self, flags);
	} else if(!(flags & TK_FLAG_DIRTY)) {
		return;	
	}
	r = self->lines;
	while (r) {
		if (r->flags & TK_FLAG_DIRTY || flags & TK_FLAG_DIRTY) {
			r->flags &= ~TK_FLAG_DIRTY;
			tk_block__draw_line(tk, self, r, flags);
		}
		r = r->next;
	}

}

