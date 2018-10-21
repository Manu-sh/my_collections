#pragma once

#ifdef FNPREFIX
	#error "shouldn't be defined"
#endif

#if !defined(TYPE) || !defined(FNTYPE)
	#error "TYPE and FNAME must be defined before including this file"
#endif

#define FNPREFIX heap
#include "../ccommon.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/*
	parent: j/2
	lchild: 2i
	rchild: 2i+1

	the root is v[1], v[0] is empty
*/

typedef struct heap {
	TYPE *v;
	bool (*lt)(TYPE,TYPE);
	int idx;
	int capacity;
} Heap;

__helper void fixUp(TYPE *v, bool (*lt)(TYPE,TYPE), int k) {

	while (k > 1 && lt(v[k>>1], v[k])) {
		SWAP(TYPE, v[k>>1], v[k]);
		k >>= 1;
	}
}

__helper void fixDown(TYPE *v, bool (*lt)(TYPE,TYPE), int p, int to) {

#if 0
	for (int j; (j = p << 1) <= to; p = j) {

		/* get max or min depending of lt() behavior */
		// j = j < to && lt(v[j], v[j+1]) ? j+1 : j;
		if (j < to && lt(v[j], v[j+1])) ++j;

		if (lt(v[p], v[j]))
			SWAP(TYPE, v[p], v[j]);
		else break;
	}
#endif

#if 1
	int j = to;
	while ((p << 1) < to) {

		j = lt(v[p<<1], v[(p<<1)+1]) ? (p<<1)+1 : p << 1;

		if (lt(v[p], v[j]))
			SWAP(TYPE, v[p], v[j]);
		else break;
		p = j;
	}

	// TODO work well but is ugly
	if (p <= to && p != j && lt(v[p], v[j]))
		SWAP(TYPE, v[p], v[j]);
#endif

}


/* INT_MAX-1 because heap start at 1 */
__always_inline	int _(maxSize)() {
	return INT_MAX-1;
}

/* provide a less_than() or greater_then() function 
respectively if you want max_heap or min_heap */

__always_inline Heap * _(new)(bool (*lt)(TYPE,TYPE)) {

	Heap *hp;

	if (!(hp = (Heap *)malloc(1 * sizeof(Heap))))
		return NULL;

	if (!(hp->v = (TYPE *)malloc((hp->capacity = 10) * sizeof(TYPE )))) {
		free(hp);
		return NULL;
	}

	hp->idx = 0;
	hp->lt  = lt;

	return hp;
}


__always_inline void _(free)(Heap *hp) { 
	if (!hp) return; 
	free(hp->v); free(hp); 
}


__always_inline bool _(isEmpty)(const Heap *hp) { 
	return hp->idx == 0; 
}


/* retrieve but not remove the root */
__always_inline TYPE  _(peek)(const Heap *hp) { 
	assert(!_(isEmpty)(hp));
	return hp->v[1]; 
}


/* number of elements that heap contains */
__always_inline int _(length)(const Heap *hp) {
	 return hp->idx;
}


__always_inline bool _(add)(Heap *hp, const TYPE e) {

	/* doubling-halving: growUp */
	if (hp->idx >= hp->capacity-1) {
		TYPE *nv = (TYPE *)realloc(hp->v, (hp->capacity*2) * sizeof(TYPE ));
		if (!nv) return false;

		hp->v = nv;
		hp->capacity *= 2;
	}

	hp->v[++hp->idx] = (TYPE )e;
	fixUp(hp->v, hp->lt, hp->idx);

	return true;

}

/* it dosn't check if heap is empty so you MUST check it */
__always_inline TYPE _(remove)(Heap *hp) {

	TYPE ret;

	SWAP(TYPE, hp->v[1], hp->v[hp->idx]);
	fixDown(hp->v, hp->lt, 1, hp->idx-1);
	ret = hp->v[hp->idx--];

	/* doubling-halving: growDown */
	if (hp->capacity/4 > hp->idx) {
		TYPE *nv = (TYPE *)realloc(hp->v, (hp->capacity/2) * sizeof(TYPE ));
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

__always_inline TYPE _(replace)(Heap *hp, const TYPE e) {

	TYPE  ret = (TYPE )e;
	assert(!_(isEmpty)(hp));

	SWAP(TYPE, ret, hp->v[1]);
	fixDown(hp->v, hp->lt, 1, hp->idx-1);
	return ret;
}