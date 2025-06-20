
#include "tk.h"

void tk_text__init(struct tk *tk, void *self_, void *data, int len)
{
	struct tk_text *self = self_;
	self->data = data;
	self->len = len;
}


void *tk_text(struct tk *tk, void *data, int len)
{
	struct tk_text *self = tk__create(tk, TK_TEXT);
	tk_text__init(tk, self, data, len);
	return self;
}


void tk_text__add(struct tk *tk, void *self_, void *data, int len)
{
	struct tk_text *self = self_;
	int nl;
	char *o;
	char *d;
	int i = self->len;
	nl = len + self->len;
	d = tk->std->alloc(nl + 1);
	if (self->data) {
		o = self->data;
		while (i > 0) {
			i--;
			d[i] = o[i];
		}
		tk->std->free(o);
	}
	self->data = d;
	d += self->len;
	o = data;
	for (i = 0; i < len; i++) {
		d[i] = o[i];
	}
	d[i] = '\0';
	self->len = len;
}


