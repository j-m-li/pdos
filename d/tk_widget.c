
#include "tk.h"

void tk_widget__init(struct tk *tk, void *self_)
{
	struct tk_text *self = self_;
}

void *tk_widget(struct tk *tk)
{
	struct tk_block *self = tk__create(tk, TK_WIDGET);
	tk_text__init(tk, self, (void*)0, 0);
	tk_widget__init(tk, self);
	return self;
}

void tk_widget__measure(struct tk *tk, void *self_, int flags)
{
}

void tk_widget__draw(struct tk *tk, void *self_, int flags)
{
}

