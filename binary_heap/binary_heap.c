#include "binary_heap.h"

#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

typedef struct heap {
	void **v;
	bool (*lt)(void*,void*);
	int idx;
	int capacity;
} Heap;

#define __helper static __attribute__((always_inline)) inline

#define SWAP(_TYPE_,A,B)          \
	do {                      \
		_TYPE_ _tmp_ = A; \
		A = B;            \
		B = _tmp_;        \
	} while(0)


/*
	parent: j/2
	lchild: 2i
	rchild: 2i+1

	the root is v[1], v[0] is empty
*/

__helper void fixUp(void **v, bool (*lt)(void*,void*), int k) {

	while (k > 1 && lt(v[k>>1], v[k])) {
		SWAP(void*, v[k>>1], v[k]);
		k >>= 1;
	}
}

__helper void fixDown(void **v, bool (*lt)(void*,void*), int p, int to) {

#if 0
	for (int j; (j = p << 1) <= to; p = j) {

		/* get max or min depending of lt() behavior */
		// j = j < to && lt(v[j], v[j+1]) ? j+1 : j;
		if (j < to && lt(v[j], v[j+1])) ++j;

		if (lt(v[p], v[j]))
			SWAP(void*, v[p], v[j]);
		else break;
	}
#endif

#if 1
	int j = to;
	while ((p << 1) < to) {

		j = lt(v[p<<1], v[(p<<1)+1]) ? (p<<1)+1 : p << 1;

		if (lt(v[p], v[j]))
			SWAP(void*, v[p], v[j]);
		else break;
		p = j;
	}

	// TODO work well but is ugly
	if (p <= to && p != j && lt(v[p], v[j]))
		SWAP(void*, v[p], v[j]);
#endif

}


/* INT_MAX-1 because heap start at 1 */
int heap_maxSize() {
	return INT_MAX-1;
}

/* provide a less_than() or greater_then() function 
respectively if you want max_heap or min_heap */

Heap * heap_new(bool (*lt)(void*,void*)) {

	Heap *hp;

	if (!(hp = (Heap *)malloc(1 * sizeof(Heap))))
		return NULL;

	if (!(hp->v = (void **)malloc((hp->capacity = 10) * sizeof(void *)))) {
		free(hp);
		return NULL;
	}

	hp->idx = 0;
	hp->lt  = lt;

	return hp;
}


void heap_free(Heap *hp) { 
	if (!hp) return; 
	free(hp->v); free(hp); 
}


bool heap_isEmpty(const Heap *hp) { 
	return hp->idx == 0; 
}


/* retrieve but not remove the root */
void * heap_peek(const Heap *hp) { 
	return heap_isEmpty(hp) ? NULL : hp->v[1]; 
}


/* number of elements that heap contains */
int heap_length(const Heap *hp) {
	 return hp->idx;
}


bool heap_add(Heap *hp, const void *e) {

	/* doubling-halving: growUp */
	if (hp->idx >= hp->capacity-1) {
		void **nv = (void **)realloc(hp->v, (hp->capacity*2) * sizeof(void *));
		if (!nv) return false;

		hp->v = nv;
		hp->capacity *= 2;
	}

	hp->v[++hp->idx] = (void *)e;
	fixUp(hp->v, hp->lt, hp->idx);
	return true;

}

/* it dosn't check if heap is empty so you MUST check it */
void * heap_remove(Heap *hp) {

	void *ret;

	SWAP(void*, hp->v[1], hp->v[hp->idx]);
	fixDown(hp->v, hp->lt, 1, hp->idx-1);
	ret = hp->v[hp->idx--];

	/* doubling-halving: growDown */
	if (hp->capacity/4 > hp->idx) {
		void **nv = (void **)realloc(hp->v, (hp->capacity/2) * sizeof(void *));
		if (!nv) return ret; /* do nothing */

		hp->v = nv;
		hp->capacity /= 2;
	}

	return ret;

}


/* replace the root with another element then fix the heap if is needed
return the previous root on success, NULL if an error is occurred.
More efficient than get followed by add, since only need to balance once, 
not twice, and appropriate for fixed-size heaps. */

void * heap_replace(Heap *hp, const void *e) {

	void * ret = (void *)e;
	if (heap_isEmpty(hp))
		return NULL;

	SWAP(void*, ret, hp->v[1]);
	fixDown(hp->v, hp->lt, 1, hp->idx-1);
	return ret;
}