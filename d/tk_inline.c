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

int  tk_inline__draw(struct tk *tk, void *self_, int start, int end)
{
	struct tk_inline *self = self_;
	int w = 0;
	if (!self || !tk) {
		return 0;
	}
	if (self->display == TK_INLINE || self->display == TK_TEXT) {
		if (self->data) {
			w = tk->draw_string(tk, 
				self->data + start, end - start);
		}
	}
	return w;
}

