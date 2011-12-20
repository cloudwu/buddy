#include "buddy.h"
#include <stdio.h>

static int
test_alloc(struct buddy *b, int sz) {
	int r = buddy_alloc(b,sz);
	printf("alloc %d (sz= %d)\n",r,sz);
	buddy_dump(b);
	return r;
}

static void
test_free(struct buddy *b, int addr) {
	printf("free %d\n",addr);
	buddy_free(b,addr);
	buddy_dump(b);
}

int
main() {
	struct buddy * b = buddy_new(4);
	buddy_dump(b);
	int m1 = test_alloc(b,2);
	int m2 = test_alloc(b,4);
	int m3 = test_alloc(b,2);
	test_free(b,m3);
	test_free(b,m1);
	test_free(b,m2);

	buddy_delete(b);
	return 0;
}