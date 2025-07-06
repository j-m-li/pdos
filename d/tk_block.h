#ifndef TK_BLOCK_H
#define TK_BLOCK_H


#ifdef __cplusplus
extern "C" {
#endif

#define TK_BLOCK__STRUCT \
	TK_INLINE__STRUCT; \
	struct tk_range *lines; \
	struct tk_inline *first_child; \
	struct tk_inline *last_child; \
	struct tk_range *selection; \
	struct tk_block *parent; \
	void (*draw)(struct tk *tk, void *self, struct tk_pos *p); \
	int (*event)(struct tk *tk, void *self, void *event); \
	int x; \
	int y; \
	int w; \
	int h

struct tk_block {
	TK_BLOCK__STRUCT;
};

void tk_block__init(struct tk *tk, void *self, int x, int y, 
		int w, int h, void *parent);
void *tk_block__add_text(struct tk *tk, void *self, char *txt, int len,
		struct tk_range *selection);
void *tk_block__add_elem(struct tk *tk, void *self, void *elem,
		struct tk_range *selection);
void tk_block__draw(struct tk *tk, void *self, struct tk_pos *pos);
void tk_block__move_cursor(struct tk *tk, void *self, 
		int dx, int dy, int flags);


#ifdef __cplusplus
}
#endif
#endif


