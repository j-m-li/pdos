
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


void tk_text__del(struct tk *tk, void *self_, int start, int len)
{
	struct tk_text *self = self_;
	int nl;
	char *o;
	char *d;
	int i;
	if (!self->data || start >= self->len) {
		return;
	}
	nl = len + self->len;
	d = self->data;
	for (i = start; (i + len) < self->len; i++) {
		d[i] = d[i + len];
	}
	d[i] = '\0';
	self->len = i;
}

int tk_text__add(struct tk *tk, void *self_, int start, void *data, int len)
{
	struct tk_text *self = self_;
	int nl;
	char *o;
	char *d;
	int i = self->len;
	int nbc = 0;

	if (!data) {
		return nbc;
	}
	nl = len + self->len;
	d = tk->std->alloc(nl + 1);
	if (!d) {
		return nbc;
	}
	if (self->data) {
		o = self->data;
		for (i = 0; i < self->len && i < start; i++) {
			d[i] = o[i];
		}
		for (; i < self->len; i++) {
			d[i + len] = o[i];
		}
		tk->std->free(o);
	}
	self->data = d;
	o = data;
	for (i = 0; i < len; i++) {
		if ((o[i] & 0xFF) <= 0x7F || (o[i] & 0xFF) >= 0xC0) {
			nbc++;
		}
		d[i + start] = o[i];
	}
	d[nl] = '\0';
	self->len = nl;
	return nbc;
}


