
#include "tk.h"


void tk_block__init(struct tk *tk, void *self_, 
		int x, int y, int w, int h, void *parent)
{
	struct tk_block *self = self_;
	self->parent = parent;
	self->x = x;
	self->y = y;
	if (w < 0) {
		self->w = tk->w;
	} else {
		self->w = w;
	}
	if (h < 0) {
		self->h = tk->w;
	} else {
		self->h = h;
	}

	self->first_child = (void*)0;
	self->last_child = (void*)0;
	self->lines = (void*)0;
	self->draw = (void*)0;
	self->event = (void*)0;
}

void *tk_block(struct tk *tk, int x, int y, int w, int h, void *parent)
{
	struct tk_block *self = tk__create(tk, TK_BLOCK);
	tk_inline__init(tk, self);
	tk_block__init(tk, self, x, y, w, h, parent);
	return self;
}


void tk_block__dirty_after(struct tk *tk, struct tk_block *self, 
		struct tk_inline *node, int offset)
{
	int f = 0;
	struct tk_range *r;
	r = self->lines;
	if (offset < 0) {
		offset = 0;
	}
	while (r) {
		if (r->start_container == node && r->start_offset <= offset
		 	&& r->end_container == node 
				&& r->end_offset >= offset) 
		{
			f = TK_FLAG_DIRTY;
		}
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
	return (void*)0;
}

void *tk_block__down(struct tk *tk, void *self_, struct tk_inline *c, int *o)
{
	struct tk_block *self = self_;
	struct tk_inline *n;
	return (void*)0;
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


void tk_block__move_cursor(struct tk *tk, void *self_, 
		int dx, int dy, int flags)
{
	struct tk_block *self = self_;
	struct tk_inline *c = self->first_child;
	struct tk_inline *nc;
	struct tk_inline *c0 = self->first_child;
	int o = 0;
	int o0 = 0;
	int fix = 0;
	int x;
	int l;
	struct tk_range *r = self->selection;
	struct tk_range *line;
	struct tk_range *line0;
	int t;

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
		if (flags & TK_FLAG_START) {
			o = r->end_offset;
			c = r->end_container;
			o0 = r->start_offset;
			c0 = r->start_container;
		} else {
			o = r->start_offset;
			c = r->start_container;
			o0 = r->end_offset;
			c0 = r->end_container;
		}
	} else {
		if (flags & TK_FLAG_START) {
			o = r->start_offset;
			c = r->start_container;
			o0 = r->end_offset;
			c0 = r->end_container;
		} else {
			o = r->end_offset;
			c = r->end_container;
			o0 = r->start_offset;
			c0 = r->start_container;
		}
	}
	while (dx > 0) {
		if (o >= c->len) {
			break;
		}
		if ((((char*)c->data)[o] & 0xFF) <= 0x7F) {
			o++;
			if (o < c->len && ((char*)c->data)[o] == '\r') {
				o++;
			}
		} else {
			o++;
			while (((int)((char*)c->data)[o] & 0xFF) < 0xC0) {
				if ((((char*)c->data)[o] & 0xFF) <= 0x7F) {
					break;
				}
				if (o >= c->len) {
					break;
				}
				o++;
			}
		}
		dx--;
	}
	while (dx < 0 || fix) {
		while (o > c->len) {
			o--;
		}
		if (o < 1) {
			break;
		}
		o--;
		if (((char*)c->data)[o] == '\r' && o > 0) {
			o--;
		}
		if ((((char*)c->data)[o] & 0xFF) > 0x7F) {
			while (o > 0 && 
				((int)((char*)c->data)[o] & 0xFF) < 0xC0) 
			{
				if ((((char*)c->data)[o] & 0xFF) <= 0x7F) {
					break;
				}
				o--;
			}
		}
		if (fix) {
			fix = 0;
			break;
		}
		dx++;
		if (dx == 0 && o > 0) {
			if ((((char*)c->data)[o] & 0xFF) == '\n') {
				//fix = 1;
			}
		}
	}
	if (dx < 0) {
		nc = tk_block__previous(tk, self, c);
		if (!nc) {
			o = 0;
		} else {
			c = nc;
			o = c->len;
			if (o > 0) {
				o--;
			}
		}
	} else if (dx > 0) {
		nc = tk_block__next(tk, self, c);
		if (!nc) {
			o = c->len;
		} else {
			c = nc;
			o = 0;
		}
	}
	if (dy) {
		line = self->lines;
		x = 0;
		while (line) {
			if (line->start_offset <= o && line->end_offset >= o) {
				if (line->next && 
						line->next->start_offset == o) 
				{
				} else {
					x = o - line->start_offset;
					break;
				}
			}
			line = line->next;
		}
	}
	line0 = line;
	while (dy > 0) {
		if (line && line->next && line->next->start_container) {
			line = line->next;
			l = line->end_offset - line->start_offset;
			if (x > l) {
				o = line->end_offset;
				if (((char*)c->data)[o-1] == '\n') {
					o--;
					if (l > 1 &&
					   ((char*)c->data)[o-1]=='\r')
					{
					 	o--;
					}
				}
			} else {
				o = line->start_offset + x;
			}
		} else {
			break;
		}
		dy--;
	}
	line = self->lines;
	while (dy < 0) {
		if (!line->next) {
			break;
		}
		while (line->next) {
			if (line->next == line0) {
				l = line->end_offset - line->start_offset;
				if (x >= l) {
					o = line->end_offset;
					if (((char*)c->data)[o-1] == '\n') {
						o--;
						if (l > 1 &&
						   ((char*)c->data)[o-1]=='\r')
						{
						 	o--;
						}
					}


				} else {
					o = line->start_offset + x;
				}
				break;
			}
			line = line->next;
		}
		if (!line->next) {
			break;
		}
		dy++;
	}

	while (dy > 0) {
		nc = tk_block__down(tk, self, c, &t);
		if (!nc) {
			break;
		}
		o = t;
		c = nc;
		dy--;
	}
	while (dy < 0) {
		nc = tk_block__up(tk, self, c, &t);
		if (!nc) {
			break;
		}
		o = t;
		c = nc;
		dy++;
	}
	if (flags & TK_FLAG_COLLAPSE) {
		r->start_container = c;
		r->end_container = c;
		r->start_offset = o; 
		r->end_offset = o;
		r->flags &= ~TK_FLAG_REVERSE;
	} else if (r->start_container == c && r->end_container == c) {
		if (flags & TK_FLAG_START) {
			if (r->flags & TK_FLAG_REVERSE) {
				if (o < o0) {
					r->flags ^= TK_FLAG_REVERSE;
				}
			} else {
				if (o > o0) {
					r->flags ^= TK_FLAG_REVERSE;
				}
			}
		} else {
			if (r->flags & TK_FLAG_REVERSE) {
				if (o > o0) {
					r->flags ^= TK_FLAG_REVERSE;
				}
			} else {
				if (o < o0) {
					r->flags ^= TK_FLAG_REVERSE;
				}
			}
		}
		if ((r->flags & TK_FLAG_REVERSE)) {
			if (flags & TK_FLAG_START) {
				r->end_offset = o;
				r->end_container = c;
				r->start_offset = o0;
				r->start_container = c0;
			} else {
				r->start_offset = o;
				r->start_container = c;
				r->end_offset = o0;
				r->end_container = c0;
			}
		} else {
			if (flags & TK_FLAG_START) {
				r->start_offset = o;
				r->start_container = c;
				r->end_offset = o0;
				r->end_container = c0;
			} else {
				r->end_offset = o;
				r->end_container = c;
				r->start_offset = o0;
				r->start_container = c0;
			}
		}
	} else {
		// FIXME
	}
}
void *tk_block__add_elem(struct tk *tk, void *self_, void *elem_,
		struct tk_range *r)
{
	struct tk_block *self = self_;
	struct tk_block *elem = elem_;
	struct tk_inline *inli = (void*)0;
	if (!elem) {
		return elem;
	}

	self->flags |= TK_FLAG_DIRTY;

	return elem;
}

void *tk_block__add_text(struct tk *tk, void *self_, char *txt, int len,
		struct tk_range *r)
{
	struct tk_block *self = self_;
	struct tk_inline *inli = (void*)0;
	if (!self || !tk || !txt) {
		return inli;
	}
	if (!self->last_child) {
		inli = tk_inline(tk);
		self->last_child = inli;
		self->first_child = inli;
	} else {
		inli = self->last_child;
	}
	if (r && r->start_container == inli && r->end_container == inli) {
		if (r->start_offset != r->end_offset) {
			tk_text__del(tk, inli, r->start_offset, 
					r->end_offset - r->start_offset);
			r->end_offset = r->start_offset;	
		}
		tk_block__dirty_after(tk, self, inli, r->start_offset - 2);
		if (len > 0) {
			tk_text__add(tk, inli, r->start_offset, txt, len);
		}
		r->start_offset += len;
		r->end_offset = r->start_offset;
	} else {
		tk_text__add(tk, inli, inli->len, txt, len);
		if (!r) {
			r = tk_range(tk, inli, len, inli, len);
			r->common_ancestor = self_;
			self->selection = r;
		}
	}
	self->flags |= TK_FLAG_DIRTY;

	return inli;
}

void tk_block__measure(struct tk *tk, void *self_, struct tk_pos *pos)
{
	struct tk_block *self = self_;
	struct tk_range *r;
	int w, h, a;
	int x;
	int o = 0;
	int y = 0;
	char *d;
	int fix = 0;
	if (!self->lines) {
		r = tk_range(tk, self->first_child, 0, self->last_child, 
				self->last_child->len);
		self->lines = r;
		r->flags |= TK_FLAG_DIRTY;
	}
	r = self->lines;
	while (r) {
		if (r->flags & (TK_FLAG_DIRTY | TK_FLAG_EMPTY) ) {
			break;
		}
		y = r->y + r->h;
		r = r->next;
	}
	if (r && (r->start_container == self->last_child)) {
		o = r->start_offset;
		d = (char*)r->start_container->data;
		while (o < self->last_child->len) {
			x = 0;
			w = self->w;
			r->start_container = self->last_child;
			r->end_container = self->last_child;
			r->start_offset = o;
			tk__damage(tk, r->x, r->y, r->w, r->h, pos);
			if (!fix && o == 0 && d[0] == '\n' && 
					self->last_child->len == 1)
			{
				fix = 1;
				tk->measure_string(tk," ", 1, &w, &h, &a);
				w = 0;
			} else if (!fix && o == 0 && 
				self->last_child->len == 2 && 
				d[0] == '\r' && d[1] == '\n') 
			{
				fix = 1;
				tk->measure_string(tk,"  ", 2, &w, &h, &a);
				w = 0;
			} else {
				o += tk_inline__measure_string(tk, 
					r->start_container,
					o,
					self->last_child->len, 
					&w, &h, &a, pos);
			}
			tk__damage(tk, x, y, w, h, pos);
			r->x = x;
			r->y = y;
			r->h = h;
			r->w = w;
			y += h;
			x += w;
			r->end_offset = o;
			r->flags |= TK_FLAG_DIRTY;
			if (o == self->last_child->len && o > 1) {
				if (d[o - 1] != '\n') {
					r = r->next;
					break;
				}
				if (!r->next) {
					r->next = tk_range(tk, 
						r->start_container, o, 
						r->start_container,
						r->start_container->len);
					r = r->next;
				} else {
					r = r->next;
					r->flags |= TK_FLAG_DIRTY;
					r->flags &= ~TK_FLAG_EMPTY;
					tk__damage(tk, r->x, r->y, 
							r->w, r->h, pos);
				}
				r->end_offset = o;
				r->start_offset = o;
				r->start_container = self->last_child;
				r->end_container = self->last_child;
				r->flags |= TK_FLAG_DIRTY;
  				r->x = pos->x;
				r->y = y;
				r->h = h;
				r->w = 0;
	
				r = r->next;
				break;
			} 
			if (o != self->last_child->len) {
				if (!r->next) {
					r->next = tk_range(tk, 
						r->start_container, o, 
						r->start_container,
						r->start_container->len);
				} else {
					r->flags |= TK_FLAG_DIRTY;
					r->flags &= ~TK_FLAG_EMPTY;
					tk__damage(tk, r->x, r->y, 
							r->w, r->h, pos);
				}
			}
			r = r->next;
		}
		while (r) {
			r->flags |= TK_FLAG_DIRTY | TK_FLAG_EMPTY;
			tk__damage(tk, r->x, r->y, r->w, r->h, pos);
			r->x = 0;
			r->y = 0;
			r->w = 0;
			r->h = 0;
			if (r != self->lines) {
				r->start_container = (void*)0;
				r->end_container = (void*)0;
			}
			r->start_offset = 0;
			r->end_offset = 0;
			r = r->next;
		}
	}
}

void tk_block__draw_line(struct tk *tk, struct tk_block *self,
		struct tk_range *r, struct tk_pos *pos_)
{	
	int c;
	struct tk_range *s = self->selection;
	struct tk_pos pos;
	pos.x = pos_->x + r->x;
	pos.y = pos_->y + r->y;
	pos.flags = pos_->flags;
	pos.col = 0;
	tk->move_to(tk, pos.x, pos.y);
	if (r->start_container == r->end_container) {
		if (s && s->start_container == s->end_container &&
				r->start_container == s->end_container &&
				s->start_offset < r->end_offset &&
				s->end_offset > r->start_offset) 
		{
			pos.col = 0;
			if (r->start_offset < s->start_offset) {
				pos.col = tk_inline__draw(tk, 
					r->start_container, 
					r->start_offset, s->start_offset, &pos);
			}
			if (s->start_offset >= r->start_offset &&
					s->end_offset <= r->end_offset)
			{
				tk->show_cursor(tk, 
						pos.x + pos.col, 
						pos.y);
				tk->move_to(tk, pos.x + pos.col, 
						pos.y);
				tk->set_rev(tk);
				pos.col += tk_inline__draw(tk, 
						s->start_container, 
						s->start_offset, 
						s->end_offset, &pos);
				tk->clr_rev(tk);
		} else if (s->start_offset >= r->start_offset) {
				tk->set_rev(tk);
				pos.col += tk_inline__draw(tk, 
						s->start_container, 
						s->start_offset, 
						r->end_offset, &pos);
				tk->clr_rev(tk);
			} else if (s->end_offset <= r->end_offset) {
				tk->show_cursor(tk, 
						pos.x + pos.col, 
						pos.y);
				tk->set_rev(tk);
				pos.col += tk_inline__draw(tk, 
						s->start_container, 
						r->start_offset, 
						s->end_offset, &pos);
				tk->clr_rev(tk);
			} else {
				tk->set_rev(tk);
				pos.col += tk_inline__draw(tk, 
						s->start_container, 
						r->start_offset, 
						r->end_offset, &pos);
				tk->clr_rev(tk);
			}
			tk->move_to(tk, pos.x + pos.col, pos.y);
			if (r->end_offset >  s->end_offset) {
				tk_inline__draw(tk, r->start_container, 
					s->end_offset, r->end_offset, &pos);
			}
		} else {
			if (s && s->start_container == s->end_container &&
				r->start_container == s->end_container &&
				s->end_offset == r->start_offset) 
			{
				tk->show_cursor(tk, 
						pos.x + pos.col, 
						pos.y);
			}
			pos.col = tk_inline__draw(tk, r->start_container, 
					r->start_offset, r->end_offset, &pos);
			if (s && s->start_container == s->end_container &&
				r->start_container == s->end_container &&
				s->start_offset == r->end_offset)
			{
					tk->show_cursor(tk, 
						pos.x + pos.col, 
						pos.y);
			}
		}
	} else {

	}
}

void tk_block__clear(struct tk *tk, void *self_, int flags)
{
	if (self_ != tk->root) {
		return;
	}
	tk->clear_rect(tk, tk->dm_left, tk->dm_top, 
			tk->dm_right - tk->dm_left,
			tk->dm_bottom - tk->dm_top);
	tk->dm_top = 10000;
	tk->dm_left = 10000;
	tk->dm_right = -10000;
	tk->dm_bottom = -10000;

}

void tk_block__draw(struct tk *tk, void *self_, struct tk_pos *pos_)
{
	struct tk_block *self = self_;
	struct tk_range *r;
	struct tk_pos pos;
	if (self->draw) {
		self->draw(tk, self, pos_);
		return;
	}
	pos.flags = pos_->flags;
	pos.col = pos_->col;
	pos.x = pos_->x + self->x;
	pos.y = pos_->y + self->y;
	if (self->flags & TK_FLAG_DIRTY) {
		tk_block__measure(tk, self, &pos);
	} else if(!(pos.flags & TK_FLAG_DIRTY)) {
		return;	
	}
	tk_block__clear(tk, self, pos.flags);
	r = self->lines;
	while (r) {
		if ((r->flags & TK_FLAG_DIRTY) || (pos.flags & TK_FLAG_DIRTY)) {
			r->flags &= ~TK_FLAG_DIRTY;
			tk_block__draw_line(tk, self, r, &pos);
		}
		r = r->next;
	}

}

