#include "buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

struct node {
	int left;
	int right;
};

struct buddy {
	int size;
	int free;
	struct node tree[1];
};

struct buddy * 
buddy_new(int level) {
	int size = 1 << level;
	struct buddy * self = malloc(sizeof(struct buddy) + sizeof(struct node) * (size * 2 - 2));
	self->size = size;
	self->free = 1;
	int i;
	for (i=1;i<size * 2 - 2;i++) {
		self->tree[i].left = i+1;
	}
	self->tree[size*2-2].left = -1;

	self->tree[0].left = self->tree[0].right = 0;
	return self;
}

void
buddy_delete(struct buddy * self) {
	free(self);
}

static inline int
_alloc_leaf(struct buddy * self) {
	int ret = self->free;
	self->free = self->tree[self->free].left;
	self->tree[ret].left = self->tree[ret].right = 0;
	return ret;
}

static inline void
_free_leaf(struct buddy * self, int index) {
	self->tree[index].left = self->free;
	self->free = index;
}

static inline int
is_pow_of_2(uint32_t x) {
	return !(x & (x-1));
}

static inline uint32_t
next_pow_of_2(uint32_t x) {
	if ( is_pow_of_2(x) )
		return x;
	x |= x>>1;
	x |= x>>2;
	x |= x>>4;
	x |= x>>8;
	x |= x>>16;
	return x+1;
}

static int
_alloc(struct buddy * self, int index, int start, int length, int size) {
	if (size > length)
		return -1;
	struct node * current = &(self->tree[index]);
	if (size == length) {
		if (current->left == 0 && current->right == 0) {
			current->left = -1;
			return start;
		}
		return -1;
	}
	if (current->left == -1) return -1;
	if (current->left != 0) {
		int r = _alloc(self, current->left, start, length/2 , size);
		if (r>=0)
			return r;
	}
	if (current->right != 0) {
		int r = _alloc(self, current->right, start + length/2 , length/2 , size);
		if (r>=0)
			return r;
	}
	if (current->left == 0) {
		int index = _alloc_leaf(self);
		current->left = index;
		return _alloc(self, index, start, length/2 , size);
	}
	if (current->right == 0) {
		int index = _alloc_leaf(self);
		current->right = index;
		return _alloc(self, index, start + length/2 , length/2 , size);
	}
	return -1;
}

int 
buddy_alloc(struct buddy * self , int size) {
	int s = (int)next_pow_of_2(size);
	int cap = self->size;
	return _alloc(self,0,0,cap,s);
}

static int
_free(struct buddy * self, int index, int start, int length, int offset) {
	assert(length > 0);
	struct node * current = &(self->tree[index]);
	if (start == offset) {
		if (current->left == -1) {
			return 1;
		}
	}
	if (offset < start + length/2) {
		index = current->left;
		assert(index > 0);
		int empty = _free(self, index, start, length/2 , offset);
		if (empty) {
			_free_leaf(self, index);
			current->left = 0;
			if (current->right == 0)
				return 1;
		}
	} else {
		index = current->right;
		assert(index > 0);
		int empty = _free(self, index, start+length/2, length/2 , offset);
		if (empty) {
			_free_leaf(self, index);
			current->right = 0;
			if (current->left == 0)
				return 1;
		}
	}
	return 0;
}

void
buddy_free(struct buddy * self, int offset) {
	if (_free(self, 0, 0, self->size, offset)) {
		self->tree[0].left = self->tree[0].right = 0;
	}
}

static int
_size(struct buddy * self, int index, int start, int length, int offset) {
	assert(length > 0);
	struct node * current = &(self->tree[index]);
	if (start == offset) {
		if (current->left == -1) {
			return length;
		}
	}
	if (offset < start + length/2) {
		index = current->left;
		assert(index > 0);
		return _size(self, index, start, length/2 , offset);
	} else {
		index = current->right;
		assert(index > 0);
		return _size(self, index, start+length/2, length/2 , offset);
	}
}

int
buddy_size(struct buddy * self, int offset) {
	return _size(self, 0 , 0 , self->size, offset);
}

static void
_dump(struct buddy * self, int index, int start, int length) {
	struct node * current = &(self->tree[index]);
	if (current->left < 0) {
		printf("[%d:%d]",start,length);
		return;
	}
	if (current->left == 0 && current->right == 0) {
		printf("(%d:%d)",start,length);
		return;
	}
	printf("(");
	if (current->left == 0) {
		printf("(%d:%d)",start,length/2);
	} else {
		_dump(self,current->left, start , length/2);
	}
	if (current->right == 0) {
		printf("(%d:%d)",start + length/2 ,length/2);
	} else {
		_dump(self,current->right, start + length/2 , length/2);
	}
	printf(")");
}

void
buddy_dump(struct buddy * self) {
	_dump(self, 0, 0, self->size);
	printf("\n");
}

