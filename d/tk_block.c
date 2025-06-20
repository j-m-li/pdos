
#include "tk.h"


void tk_block__init(struct tk *tk, void *self_, 
		int x, int y, int w, int h)
{
	struct tk_block *self = self_;
	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
	self->first_child = 0;
	self->last_child = 0;
	self->lines = 0;
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

void *tk_block__add_text(struct tk *tk, void *self_, char *txt, int len)
{
	struct tk_block *self = self_;
	struct tk_inline *inli = 0;
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
	r = self->lines;
	while (r) {
		if (r->flags & TK_FLAG_DIRTY) {
			break;
		}
		r = r->next;
	}
	if (r && (r->start_container == self->last_child)) {

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

