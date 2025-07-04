#ifndef TK_RANGE_H
#define TK_RANGE_H


#ifdef __cplusplus
extern "C" {
#endif

struct tk_range {
	TK__STRUCT;
	struct tk_inline *start_container;
	int start_offset;
	struct tk_inline *end_container;
	int end_offset;
	struct tk_inline *common_ancestor;
	struct tk_range *next;
	int flags;
	int x;
	int y;
	int h;
	int w;
};


void tk_range__init(struct tk *tk, void *self, struct tk_inline *sc,
		int so, struct tk_inline *ec, int eo);

#ifdef __cplusplus
}
#endif
#endif


