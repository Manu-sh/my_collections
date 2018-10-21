#include "../ccommon.c"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	TYPENAME data;
} Node;

struct STRUCT {
	Node *head;
	Node *tail; /* point to a preallocated block without data */
	int length; /* for obvious reasons the tail isn't count as part of length */
};

/* b should be an empty node only data field should contain non-null value */
#define NODE_PUSH_BACK(_ANODE_,_BNODE_)  \
	do {                             \
		_BNODE_->prev = _ANODE_; \
		_ANODE_->next = _BNODE_; \
	} while(0)                       \



#define NODE_PUSH_FRONT(_ANODE_,_BNODE_) \
	do {                             \
		_BNODE_->next = _ANODE_; \
		_ANODE_->prev = _BNODE_; \
	} while(0)                       \


/* insert c between a and b, c should be an empty node (except for data) */
#define NODE_INSERT_BETWEEN(_ANODE_,_BNODE_,_CNODE_)  \
	do {                                          \
		_CNODE_->next = _BNODE_;              \
		_BNODE_->prev = _CNODE_;              \
		_CNODE_->prev = _ANODE_;              \
		_ANODE_->next = _CNODE_;              \
	} while(0)                                    \


__always_inline int _(maxSize)() { return INT_MAX-1; }

__always_inline STRUCT * _(new()) {

	STRUCT *ls;
	if (!(ls = (STRUCT *)calloc(1, sizeof(STRUCT))))
		return NULL;

	/* pre-allocation */
	if (!(ls->head = (Node *)calloc(1, sizeof(Node)))) {
		free(ls);
		return NULL;
	}

	ls->tail = ls->head;
	return ls;
}

__always_inline void _(free)(STRUCT *ls) {

	Node *p;

	if (!ls) return;

	for (p = ls->head; p != ls->tail;) {
	       	p = p->next;
		free(p->prev);
	}

	free(p), free(ls);
}

__always_inline int  _(length)(const STRUCT *ls) { 
	return ls->length;
}

__always_inline bool _(isEmpty)(const STRUCT *ls) { 
	/* return ls->length == 0; */
	return !ls->head->next; /* if there isn't another preallocated blk head point to tail and is itself the preallocated block */
}

__always_inline TYPENAME _(front)(const STRUCT *ls) { 
	return ls->head->data;  /* there is always at least 1 blk */
}

__always_inline TYPENAME _(back)(const STRUCT *ls) { 
#if 0
	return linkedlist_isEmpty(ls) ? NULL : ls->tail->prev->data;  /* prev could be NULL */
#endif

	assert(!_(isEmpty)(ls));
	return ls->tail->prev->data;
}

__always_inline bool _(pushBack)(STRUCT *ls, const TYPENAME e) {

	Node *next;

	/* pre-allocation */
	if (!(next = (Node *)calloc(1, sizeof(Node))))
		return false;

	/* save into a previous preallocated block */
	ls->tail->data = (TYPENAME)e;
	NODE_PUSH_BACK(ls->tail, next);

	/* tail must point to a preallocated block without data */
	ls->tail = next;

	++ls->length;
	return true;
}

__always_inline bool _(pushFront)(STRUCT *ls, const TYPENAME e) {

	Node *prev;

	if (!(prev = (Node *)calloc(1, sizeof(Node))))
		return false;

	prev->data = (TYPENAME)e;
	NODE_PUSH_FRONT(ls->head, prev);
	ls->head = prev;

	++ls->length;
	return true;
}

__always_inline TYPENAME _(popBack)(STRUCT *ls) {

	TYPENAME ret;
	Node *tofree;

	/* check tail->prev (if tail == head list is empty and
	   heap->prev->data result in an illegal access) */
	assert( !_(isEmpty)(ls) );

	/* tail point to a pre-allocate block without data (always) */
	tofree = ls->tail;
    	ret    = ls->tail->prev->data;

	ls->tail->prev->next = NULL;
	/* ls->tail->prev->data = NULL; */

	/* tail must point to a preallocated block */
	ls->tail             = ls->tail->prev;
	free(tofree);

	--ls->length;
	return ret;
}

__always_inline TYPENAME _(popFront)(STRUCT *ls) {

	/* there is always at least one block */
	TYPENAME ret = ls->head->data;
	Node *tofree = ls->head;

	assert(!_(isEmpty)(ls)); /* unecessay */

	if (!ls->head->next) { /* don't free nothing if is the last blk */
		/* ls->head->data = -1; */
		return ret;
	}

	ls->head       = ls->head->next;
	ls->head->prev = NULL;
	free(tofree);

	--ls->length;
	return ret;
}


/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */

__always_inline STRUCT * _(merge)(STRUCT *a, STRUCT **b) {

	Node *n;

	if (!a || !b || !*b)
		return NULL;

	/* in the worst case tail point to head (that is the pre-allocated block) */
	n = a->tail;

	/* remove tail (the preallocated block) */
	if (a->length) {
		n = a->tail->prev;
		free(a->tail);
	}

	/* merge */
	NODE_PUSH_BACK(n, (*b)->head);
	a->tail = (*b)->tail;

	/* remember the tail isn't count as part of length */
	a->length += (*b)->length;

	/* free the container structure STRUCT (not the nodes) */
	free(*b); *b = NULL;
	return a;
}