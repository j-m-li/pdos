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
void tk_inline__draw(struct tk *tk, void *self, int start, int end);

#ifdef __cplusplus
}
#endif
#endif


