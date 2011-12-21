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

static void
test_size(struct buddy *b, int addr) {
	int s = buddy_size(b,addr);
	printf("size %d (sz = %d)\n",addr,s);
}

int
main() {
	struct buddy * b = buddy_new(5);
	buddy_dump(b);
	int m1 = test_alloc(b,4);
	test_size(b,m1);
	int m2 = test_alloc(b,9);
	test_size(b,m2);
	int m3 = test_alloc(b,3);
	test_size(b,m3);
	int m4 = test_alloc(b,2);
	test_size(b,m4);
	int m5 = test_alloc(b,1);
	test_size(b,m5);

	test_free(b,m3);
	test_free(b,m1);
	test_free(b,m5);
	test_free(b,m2);
	test_free(b,m4);

	buddy_delete(b);
	return 0;
}
