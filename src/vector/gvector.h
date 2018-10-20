#pragma once

#ifdef FNPREFIX
	#error "shouldn't be defined"
#endif

#if !defined(TYPE) || !defined(FNTYPE)
	#error "TYPE and FNAME must be defined before including this file"
#endif

#define FNPREFIX vector
#include "../ccommon.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct vector {
	TYPE *v;
	int idx;
	int capacity;
} Vector;

__always_inline TYPE * _(data)(Vector *vct) { return vct->v; }
__always_inline bool _(isEmpty)(Vector *vct) { return vct->idx == 0; }
__always_inline int  _(length)(Vector *vct)  { return vct->idx; }
__always_inline int  _(maxSize)() { return INT_MAX; }
__always_inline void _(clear)(Vector *vct) { vct->idx = 0; }
__always_inline int  _(capacity)(Vector *vct) { return vct->capacity; }

// vector random access
__always_inline TYPE _(access)(Vector *vct, int at) { return vct->v[at]; }
__always_inline void _(assign)(Vector *vct, int at, TYPE e) { vct->v[at] = e; }


__always_inline bool _(resize)(Vector *vct, int len) {

	TYPE *nv = (TYPE *)realloc(vct->v, len * sizeof(TYPE ));
	if (!nv) return false;

	vct->v = nv;
	vct->capacity = len;
	vct->idx = vct->idx >= len ? len : vct->idx;

	return true;
}

__always_inline bool _(shrinkToFit)(Vector *vct) {
	TYPE *nv = (TYPE *)realloc(vct->v, (vct->idx+1) * sizeof(TYPE ));
	if (!nv) return false;
	vct->capacity = vct->idx+1;
	return true;
}

__always_inline Vector * _(new)() {

	Vector *vct;

	if (!(vct = (Vector *)calloc(1, sizeof(Vector))))
		return NULL;

	vct->capacity  = 10;
	if (!(vct->v = (TYPE *)malloc(vct->capacity*sizeof(TYPE )))) {
		free(vct);
		return NULL;
	}

	return vct;
}

__always_inline void _(free)(Vector *vct) {
	if (!vct) return;
	free(vct->v);
	free(vct);
}

__always_inline bool _(pushBack)(Vector *vct, TYPE e) {

	/* doubling-halving: growUp */
	if (vct->idx >= vct->capacity-1) {
		TYPE *nv = (TYPE *)realloc(vct->v, (vct->capacity*2) * sizeof(TYPE ));
		if (!nv) return false;

		vct->v = nv;
		vct->capacity *= 2;
	}

	vct->v[vct->idx++] = (TYPE )e;
	return true;
}

/* it dosn't check if heap is empty so you MUST check it calling vector_isEmpty() */
__always_inline TYPE  _(popBack)(Vector *vct) {

	TYPE ret = vct->v[--vct->idx];
       
	/* doubling-halving: growDown */
	if (vct->capacity/4 > vct->idx) {
		TYPE *nv = (TYPE *)realloc(vct->v, (vct->capacity/2) * sizeof(TYPE ));
		if (!nv) return ret; /* do nothing */

		vct->v = nv;
		vct->capacity /= 2;
	}

	return ret;
}

// get back the old element
__always_inline TYPE _(replace)(Vector *vct, int at, TYPE e) {
	TYPE ret = vct->v[at];
	return (vct->v[at] = e), ret;
}