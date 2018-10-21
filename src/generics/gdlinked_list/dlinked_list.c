#include "../ccommon.c"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/* TODO provide an iterator C++ style for support an easy insertion */

typedef struct node {
	struct node *next;
	struct node *prev;
	TYPENAME data;
} Node;

struct STRUCT {
	Node *head;
	Node *tail; /* point to a preallocated block without data */
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

__always_inline bool _(isEmpty)(const STRUCT *ls) { 
	return !ls->head->next; /* if there isn't another preallocated blk head point to tail and is itself the preallocated block */
}

__always_inline Node * _(front)(const STRUCT *ls) { 
	return ls->head;  /* there is always at least 1 blk */
}

__always_inline  Node * _(back)(const STRUCT *ls) {
	return ls->tail->prev;
}

__always_inline  Node * _(pushBack)(STRUCT *ls, const TYPENAME e) {

	Node *ret, *next;

	/* pre-allocation */
	if (!(next = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	/* save into a previous preallocated block */
	ls->tail->data = (TYPENAME)e;
	ret = ls->tail;

	NODE_PUSH_BACK(ls->tail, next);

	/* tail must point to a preallocated block without data */
	ls->tail = next;
	return ret;
}

__always_inline Node * _(insertAfterNode)(STRUCT *ls, Node *a, const TYPENAME e) {

	Node *n;

	assert(a);
	if (!a->next) return linkedlist_pushBack(ls, e); /* there ins't another node, it's a simple pushBack */

	if (!(n = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	NODE_INSERT_BETWEEN(a, a->next, n);
	n->data = (TYPENAME)e;
	return n;
}

__always_inline Node * _(pushFront)(STRUCT *ls, const TYPENAME e) {

	Node *prev;

	if (!(prev = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	prev->data = (TYPENAME)e;
	NODE_PUSH_FRONT(ls->head, prev);

	return (ls->head = prev);
}


__always_inline Node * _(insertBeforeNode)(STRUCT *ls, Node *a, const TYPENAME e) { 

	Node *n;
	assert(a);

	if (!a->prev) return linkedlist_pushFront(ls, e); /* there ins't another node it's a simple pushFront */

	if (!(n = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	NODE_INSERT_BETWEEN(a->prev, a, n);
	n->data = (void *)e;
	return n;
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
	ls->tail = ls->tail->prev;
	free(tofree);

	return ret;
}

/* remove the node between two nodes a and b and return is value */
__helper TYPENAME node_removeBetween(Node *a, Node *b) {
	Node *tofree = a->next;
	TYPENAME ret = tofree->data;
	a->next = b;
	b->prev = a;
	free(tofree);
	return ret;
}

__always_inline TYPENAME linkedlist_removeAfterNode(STRUCT *ls, Node *a) {
	assert(a);
	assert(a->next && a->next->next);
	return node_removeBetween(a, a->next->next);
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
	return ret;
}

__always_inline TYPENAME _(removeBeforeNode)(STRUCT *ls, Node *a) {
	assert(a);
	assert(a->prev);
	return a->prev->prev ? node_removeBetween(a->prev->prev, a) : node_removeBetween(a->prev, a->next);
}

__always_inline TYPENAME linkedlist_replaceNodeContent(Node *n, TYPENAME e) {
	TYPENAME ret;
	assert(n);
	ret = n->data;
	return (n->data = (void *)e), ret;
}