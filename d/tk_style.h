#ifndef TK_STYLE_H
#define TK_STYLE_H


#ifdef __cplusplus
extern "C" {
#endif

enum {
	TK_PERCENT = 0xA0000000,
	TK_FULL = 0xA0002710, /* 100% = 0xA0000000 + 10000*/
	TK_BLACK = 0x7F000000, /* ARGB */
	TK_SILVER = 0x7FC0C0C0,
	TK_GRAY = 0x7F808080,
	TK_GREY = 0x7F808080,
	TK_WHITE = 0x7FFFFFFF,
	TK_MAROON = 0x7F800000,
	TK_RED = 0x7FFF0000,
	TK_PURPLE = 0x7F800080,
	TK_FUCHSIA = 0x7FFF00FF,
	TK_GREEN = 0x7F008000,
	TK_LIME = 0x7F00FF00,
	TK_OLIVE = 0x7F808000,
	TK_YELLOW = 0x7FFFFF00,
	TK_NAVY = 0x7F000080,
	TK_BLUE = 0x7F0000FF,
	TK_TEAL = 0x7F008080,
	TK_AQUA = 0x7F00FFFF,
	TK_ORANGE = 0x7FFFA500,
	TK_DARKGRAY = 0x7FA9A9A9,
	TK_DARKGREY = 0x7FA9A9A9,
	TK_DIMGRAY = 0x7F696969,
	TK_DIMGREY = 0x7F696969,
	TK_GAINSBORO = 0x7FDCDCDC,
	TK_LIGHTGRAY = 0x7FD3D3D3,
	TK_LIGHTGREY = 0x7FD3D3D3,
	TK_WHITESMOKE = 0x7FF5F5F5,
	TK_TRANSPARENT = 0x4000000,
	TK_INHERIT = 0x80000000,
	TK_HIDDEN = 0x80000001,
	TK_AUTO = 0x80000002,
	TK_NONE = 0x80000003,
	TK_INITIAL = 0x80000004,
	TK_SCROLL = 0x80000005,
	TK_VISIBLE = 0x80000006,
	TK_SOLID = 0x20000000,
	TK_OUTSET,
	TK_INSET,
	TK_GROOVE,
	TK_STATIC,
	TK_ABSOLUTE,
	TK_FIXED,
	TK_RELATIVE,
	TK_STICKY,
	TK_LEFT,
	TK_RIGHT,
	TK_CENTER,
	TK_JUSTIFY,
	TK_BASELINE,
	TK_TEXT_TOP,
	TK_TEXT_BOTTOM,
	TK_BOTTOM,
	TK_TOP,
	TK_SUB,
	TK_SUPER,
	TK_RTL,
	TK_LTR,
	TK_UNDERLINE,
	TK_LINE_THROUGH,
	TK_NOWRAP,
	TK_PRE,
	TK_NORMAL,
	TK_ITALIC,
	TK_SERIF,
	TK_SANS_SERIF,
	TK_MONOSPACE,
	TK_CURSIVE,
	TK_TEXT,
	TK_TAB,
	TK_WIDGET,
	TK_RANGE,
	TK_INLINE,
	TK_BLOCK,
	TK_CONTENTS,
	TK_FLEX,
	TK_GRID,
	TK_INLINE_BLOCK,
	TK_INLINE_FLEX,
	TK_INLINE_GRID,
	TK_INLINE_TABLE,
	TK_LIST_ITEM,
	TK_RUN_IN,
	TK_TABLE,
	TK_TABLE_CAPTION,
	TK_TABLE_COLUMN_GROUP,
	TK_TABLE_HEADER_GROUP,
	TK_TABLE_FOOTER_GROUP,
	TK_TABLE_ROW_GROUP,
	TK_TABLE_CELL,
	TK_TABLE_COLUMN,
	TK_TABLE_ROW
};

struct tk_style {
	int color;
	int background_color;
	int border_style;
	int border_witdh;
	int border_color;
	int margin_top;
	int margin_right;
	int margin_bottom;
	int margin_left;
	int padding;
	int left;
	int top;
	int height;
	int width;
	int min_width;
	int max_width;
	int min_height;
	int max_height;
	int text_align;
	int vertical_align;
	int direction;
	int text_decoration;
	int line_height;
	int white_space;
	int font_family;
	int font_style;
	int font_size;
	int display;
	int position;
	int overflow;
};

#ifdef __cplusplus
}
#endif
#endif

