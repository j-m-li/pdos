#ifndef TK_H
#define TK_H

#include "std.h"

struct tk;

#define TK__STRUCT \
	int display

struct tk__call {
	void (*dispose)(struct tk *tk, void *self);
};

struct tk {
	struct tk__call *call;
	TK__STRUCT;
	struct std *std;
	int w;
	int h;
	void (*draw_string)(struct tk *tk, char *txt, int len);
	void (*move_to)(struct tk *tk, int x, int y);
};

enum {
	TK_FLAG_DIRTY = 0x80000000
};

#include "tk_style.h"
#include "tk_text.h"
#include "tk_inline.h"
#include "tk_range.h"
#include "tk_block.h"

#ifdef __cplusplus
extern "C" {
#endif


void *tk__init(struct std *std);
void *tk__create(struct tk *tk, int type);
void *tk_text(struct tk *tk, void *data, int len);
void *tk_inline(struct tk *tk);
void *tk_range(struct tk *tk, struct tk_inline *sc, int so, 
		struct tk_inline *ec, int eo);
void *tk_block(struct tk *tk, int x, int y, int w, int h);
#ifdef __cplusplus
}
#endif
#endif

