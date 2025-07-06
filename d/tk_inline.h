#ifndef TK_INLINE_H
#define TK_INLINE_H


#ifdef __cplusplus
extern "C" {
#endif

#define TK_INLINE__STRUCT \
	TK_TEXT__STRUCT; \
	struct tk_inline *next; \
	int flags

struct tk_inline {
	TK_INLINE__STRUCT;
};

void tk_inline__init(struct tk *tk, void *self);
int tk_inline__draw(struct tk *tk, void *self, int start, 
		int end, struct tk_pos *pos);
int  tk_inline__measure_string(struct tk *tk, void *self_, int start,
                int end, int *w, int *h, int *a, struct tk_pos *pos);

#ifdef __cplusplus
}
#endif
#endif


