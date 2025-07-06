#ifndef TK_H
#define TK_H

#include "std.h"

#define TK__STRUCT \
	int display

struct tk__call {
	void (*dispose)(void *tk, void *self);
};

struct tk_pos {
	int x;
	int y;
	int flags;
	int col;
};

struct tk {
	TK__STRUCT;
	struct tk__call *call;
	struct std *std;
	struct tk_block *root;
	int w;
	int h;
	struct tk_pos pos;
	int cursor_x;
	int cursor_y;
        int dm_top; 
        int dm_left; 
        int dm_bottom;
        int dm_right;
	int (*draw_string)(struct tk *tk, char *txt, int len);
	int (*measure_string)(struct tk *tk, char *txt, int len, 
			int *width, int *height, int *ascent);
	void (*move_to)(struct tk *tk, int x, int y);
	void (*clear_rect)(struct tk *tk, int x, int y, int w, int h);
	void (*show_cursor)(struct tk *tk, int x, int y);
	void (*hide_cursor)(struct tk *tk);
	void (*set_rev)(struct tk *tk);
	void (*clr_rev)(struct tk *tk);
	void (*print_status)(struct tk *tk, char *txt, int a, int b);
};

enum {
	TK_FLAG_DIRTY = 0x80000000,
	TK_FLAG_REVERSE = 0x40000000,
	TK_FLAG_COLLAPSE = 0x20000000,
	TK_FLAG_END = 0x10000000,
	TK_FLAG_START = 0x08000000,
	TK_FLAG_EMPTY = 0x04000000
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
void tk__damage(struct tk *tk, int x, int y, int w, int h, struct tk_pos *pos);

void *tk_text(struct tk *tk, void *data, int len);
void *tk_inline(struct tk *tk);
void *tk_range(struct tk *tk, struct tk_inline *sc, int so, 
		struct tk_inline *ec, int eo);

void *tk_block(struct tk *tk, int x, int y, int w, int h, void *parent);

void *tk_widget(struct tk *tk);
void tk_widget__init(struct tk *tk, void *self);

#ifdef __cplusplus
}
#endif
#endif

