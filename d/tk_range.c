#include "tk.h"

void tk_range__init(struct tk *tk, void *self_, struct tk_inline *sc,
		int so, struct tk_inline *ec, int eo)
{
	struct tk_range *self = self_;
	self->start_container = sc;
	self->end_container = ec;
	self->start_offset = so;
	self->end_offset = eo;
	self->flags = 0;
	self->next = (void*)0;
}

void *tk_range(struct tk *tk, struct tk_inline *sc,
		int so, struct tk_inline *ec, int eo)
{
	struct tk_range *self = tk__create(tk, TK_RANGE);
	tk_range__init(tk, self, sc, so, ec, eo);
	return self;
}


