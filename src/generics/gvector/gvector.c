#include "../ccommon.c"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

struct STRUCT {
	TYPENAME *v;
	int idx;
	int capacity;
};

__always_inline TYPENAME * _(data)(const STRUCT *vct) { return vct->v; }
__always_inline bool _(isEmpty)(const STRUCT *vct) { return vct->idx == 0; }
__always_inline int  _(length)(const STRUCT *vct)  { return vct->idx; }
__always_inline int  _(maxSize)() { return INT_MAX; }
__always_inline void _(clear)(STRUCT *vct) { vct->idx = 0; }
__always_inline int  _(capacity)(STRUCT *vct) { return vct->capacity; }

/* vector random access */
__always_inline TYPENAME _(access)(const STRUCT *vct, int at) { return vct->v[at]; }
__always_inline void _(assign)(STRUCT *vct, int at, const TYPENAME e) { vct->v[at] = (TYPENAME)e; }

__always_inline bool _(resize)(STRUCT *vct, int len) {

	TYPENAME *nv = (TYPENAME *)realloc(vct->v, len * sizeof(TYPENAME ));
	if (!nv) return false;

	vct->v = nv;
	vct->capacity = len;
	vct->idx = vct->idx >= len ? len : vct->idx;

	return true;
}

__always_inline bool _(shrinkToFit)(STRUCT *vct) {
	TYPENAME *nv = (TYPENAME *)realloc(vct->v, (vct->idx+1) * sizeof(TYPENAME ));
	if (!nv) return false;
	vct->capacity = vct->idx+1;
	return true;
}

__always_inline STRUCT * _(new)() {

	STRUCT *vct;

	if (!(vct = (STRUCT *)calloc(1, sizeof(STRUCT))))
		return NULL;

	vct->capacity  = 10;
	if (!(vct->v = (TYPENAME *)malloc(vct->capacity*sizeof(TYPENAME )))) {
		free(vct);
		return NULL;
	}

	return vct;
}

__always_inline void _(free)(STRUCT *vct) {
	if (!vct) return;
	free(vct->v);
	free(vct);
}

__always_inline bool _(pushBack)(STRUCT *vct, const TYPENAME e) {

	/* doubling-halving: growUp */
	if (vct->idx >= vct->capacity-1) {
		TYPENAME *nv = (TYPENAME *)realloc(vct->v, (vct->capacity*2) * sizeof(TYPENAME ));
		if (!nv) return false;

		vct->v = nv;
		vct->capacity *= 2;
	}

	vct->v[vct->idx++] = (TYPENAME )e;
	return true;
}

bool _(pushBackAll)(STRUCT *vct, const TYPENAME *v, int len) {

	const TYPENAME *end = v+len;

	if (vct->idx+len >= vct->capacity) {

		TYPENAME *nv = (TYPENAME *)realloc(vct->v, (vct->capacity+len) * sizeof(TYPENAME ));
		if (!nv) return false;

		vct->v = nv;
		vct->capacity += len;
	}

	
	for (; v != end; ++v)
		vct->v[vct->idx++] = *((TYPENAME *)v); /* necessary cast for pointers */

	return true;
}

/* it dosn't check if vector is empty so you MUST check it calling vector_isEmpty() */
__always_inline TYPENAME _(popBack)(STRUCT *vct) {

	TYPENAME ret = vct->v[--vct->idx];
       
	/* doubling-halving: growDown */
	if (vct->capacity/4 > vct->idx) {
		TYPENAME *nv = (TYPENAME *)realloc(vct->v, (vct->capacity/2) * sizeof(TYPENAME ));
		if (!nv) return ret; /* do nothing */

		vct->v = nv;
		vct->capacity /= 2;
	}

	return ret;
}

/* get back the old element */
__always_inline TYPENAME _(replace)(STRUCT *vct, int at, const TYPENAME e) {
	TYPENAME ret = vct->v[at];
	return (vct->v[at] = (TYPENAME)e), ret;
}
