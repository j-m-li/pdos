#ifndef TK_TEXT_H
#define TK_TEXT_H


#ifdef __cplusplus
extern "C" {
#endif

#define TK_TEXT__STRUCT \
	TK__STRUCT; \
	void *data; \
	int len	

struct tk_text {
	TK_TEXT__STRUCT;
};

void tk_text__init(struct tk *tk, void *self, void *data, int len);
void tk_text__add(struct tk *tk, void *self, void *data, int len);

#ifdef __cplusplus
}
#endif
#endif


