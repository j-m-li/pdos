
struct std {
	int (*print)(char *buf, int l);
	void (*exit)(char *txt);
	void (*flush)();
	void *(*alloc)(int size);
	void (*free)(void *m);
};

