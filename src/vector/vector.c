#include "vector.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct vector {
	void **v;
	int idx;
	int capacity;
} Vector;

// INT_MAX*sizeof(int) = 8589934588: i have no ram to test it !!!
// numfmt --to=si 8589934588 8,6G

void * vector_data(Vector *vct) { return vct->v; }
bool   vector_isEmpty(Vector *vct) { return vct->idx == 0; }
int    vector_length(Vector *vct)  { return vct->idx; }
int    vector_maxSize() { return INT_MAX; }
void   vector_clear(Vector *vct) { vct->idx = 0; }
int    vector_capacity(Vector *vct) { return vct->capacity; }

// vector random access
void * vector_access(Vector *vct, int at) { return vct->v[at]; }
void   vector_assign(Vector *vct, int at, void *e) { vct->v[at] = e; }


bool vector_resize(Vector *vct, int len) {

	void **nv = (void **)realloc(vct->v, len * sizeof(void *));
	if (!nv) return false;

	vct->v = nv;
	vct->capacity = len;
	vct->idx = vct->idx >= len ? len : vct->idx;

	return true;
}

bool vector_shrinkToFit(Vector *vct) {
	void **nv = (void **)realloc(vct->v, (vct->idx+1) * sizeof(void *));
	if (!nv) return false;
	vct->capacity = vct->idx+1;
	return true;
}



Vector * vector_new() {

	Vector *vct;

	if (!(vct = (Vector *)calloc(1, sizeof(Vector))))
		return NULL;

	vct->capacity  = 10;
	if (!(vct->v = (void **)malloc(vct->capacity*sizeof(void *)))) {
		free(vct);
		return NULL;
	}

	return vct;
}

void vector_free(Vector *vct) {
	if (!vct) return;
	free(vct->v);
	free(vct);
}

bool vector_pushBack(Vector *vct, void *e) {

	/* doubling-halving: growUp */
	if (vct->idx >= vct->capacity-1) {
		void **nv = (void **)realloc(vct->v, (vct->capacity*2) * sizeof(void *));
		if (!nv) return false;

		vct->v = nv;
		vct->capacity *= 2;
	}

	vct->v[vct->idx++] = (void *)e;
	return true;
}

/* it dosn't check if heap is empty so you MUST check it calling vector_isEmpty() */
void * vector_popBack(Vector *vct) {

	void *ret = vct->v[--vct->idx];
       
	/* doubling-halving: growDown */
	if (vct->capacity/4 > vct->idx) {
		void **nv = (void **)realloc(vct->v, (vct->capacity/2) * sizeof(void *));
		if (!nv) return ret; /* do nothing */

		vct->v = nv;
		vct->capacity /= 2;
	}

	return ret;
}

// get back the old element
void * vector_replace(Vector *vct, int at, void *e) {
	void *ret = vct->v[at];
	return (vct->v[at] = e), ret;
}
