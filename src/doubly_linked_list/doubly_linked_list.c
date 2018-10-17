#include "doubly_linked_list.h"

#include <stdlib.h>
#include <stdbool.h>

// TODO provide an iterator C++ style for support an easy insertion
// TODO linkedlist_maxSize() i have no ram to test it

typedef struct node {
	struct node *next;
	struct node *prev;
	void *data;
} Node;

typedef struct linkedlist {
	Node *head;
	Node *tail; /* point to a preallocated block without data */
	int length; /* for obvious reasons the tail isn't count as part of length */
} LinkedList;

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

/* remove the node between two nodes a and b and return is value */
static void * node_removeBetween(Node *a, Node *b) {
	Node *tofree = a->next;
	void *ret    = tofree->data;
	a->next = b;
	b->prev = a;
	free(tofree);
	return ret;
}

// TODO adjust
Node * linkedlist_pushBack(LinkedList *ls, const void *e);
Node * linkedlist_pushFront(LinkedList *ls, const void *e);


/* linear search, return the content of the first element in case of success, otherwise NULL */
Node * find(const LinkedList *ls, bool (*eq)(void*,void*), const void *e) {

	for (Node *p = ls->head; p; p = p->next)
		if (eq(p->data, (void *)e))
			return p;
	return NULL;
}

void * linkedlist_removeAfterNode(LinkedList *ls, Node *a) {

	if (!a->next || !a->next->next) 
		return NULL;

	--ls->length;
	return node_removeBetween(a, a->next->next);
}

void * linkedlist_removeBeforeNode(LinkedList *ls, Node *a) {

	if (!a->prev)
	       	return NULL;

	--ls->length;
	return a->prev->prev ? node_removeBetween(a->prev->prev, a) : node_removeBetween(a->prev, a->next);
}

void * linkedlist_replaceNodeContent(Node *n, const void *e) {
	void *ret = n->data;
	return (n->data = (void *)e), ret;
}

Node * linkedlist_insertAfterNode(LinkedList *ls, Node *a, const void *e) {

	Node *n;
	if (!a->next)
		return linkedlist_pushBack(ls, e);

	if (!(n = (Node *)calloc(1, sizeof(Node))))
		return false;

	NODE_INSERT_BETWEEN(a, a->next, n);
	n->data = (void *)e;
	++ls->length;

	return n;
}

Node * linkedlist_insertBeforeNode(LinkedList *ls, Node *a, const void *e) { 

	Node *n;
	if (!a->prev)
		return linkedlist_pushFront(ls, e);

	if (!(n = (Node *)calloc(1, sizeof(Node))))
		return false;

	NODE_INSERT_BETWEEN(a->prev, a, n);
	n->data = (void *)e;
	++ls->length;

	return n;
}


LinkedList * linkedlist_new() {

	LinkedList *ls;
	if (!(ls = (LinkedList *)calloc(1, sizeof(LinkedList))))
		return NULL;

	/* pre-allocation */
	if (!(ls->head = (Node *)calloc(1, sizeof(Node)))) {
		free(ls);
		return NULL;
	}

	ls->tail = ls->head;
	return ls;
}

void linkedlist_free(LinkedList *ls) {

	Node *p;

	if (!ls) return;

	for (p = ls->head; p != ls->tail;) {
	       	p = p->next;
		free(p->prev);
	}

	free(p), free(ls);
}

int  linkedlist_length(LinkedList *ls) { 
	return ls->length;
}

bool linkedlist_isEmpty(LinkedList *ls) { 
	return ls->length == 0;
}

void * linkedlist_front(LinkedList *ls) { 
	return ls->head->data;  /* there is always at least 1 blk */
}

void * linkedlist_back(LinkedList *ls) { 
	return linkedlist_isEmpty(ls) ? NULL : ls->tail->prev->data;  /* prev could be NULL */
}

Node * linkedlist_pushBack(LinkedList *ls, const void *e) {

	Node *next, *ret = ls->tail;

	/* pre-allocation */
	if (!(next = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	/* save into a previous preallocated block */
	ls->tail->data = (void *)e;
	NODE_PUSH_BACK(ls->tail, next);

	/* tail must point to a preallocated block without data */
	ls->tail = next;

	++ls->length;
	return ret;
}

Node * linkedlist_pushFront(LinkedList *ls, const void *e) {

	Node *prev;

	if (!(prev = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	prev->data = (void *)e;
	NODE_PUSH_FRONT(ls->head, prev);
	ls->head = prev;

	++ls->length;
	return ls->head;
}

void * linkedlist_popBack(LinkedList *ls) {

	void *ret;
	Node *tofree;

	/* check tail->prev (if tail == head list is empty and
	   heap->prev->data result in an illegal access) */
	if (linkedlist_isEmpty(ls))
		return NULL;

	/* tail point to a pre-allocate block without data (always) */
	tofree = ls->tail;
    	ret    = ls->tail->prev->data;

	ls->tail->prev->next = NULL;
	ls->tail->prev->data = NULL;

	/* tail must point to a preallocated block */
	ls->tail             = ls->tail->prev;
	free(tofree);

	--ls->length;
	return ret;
}

void * linkedlist_popFront(LinkedList *ls) {

	/* there is always at least one block */
	void *ret    = ls->head->data;
	Node *tofree = ls->head;

	if (!ls->head->next) { /* don't free nothing if is the last blk */
		ls->head->data = NULL;
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

LinkedList * linkedlist_merge(LinkedList *a, LinkedList **b) {

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

	/* free the container structure LinkedList (not the nodes) */
	free(*b); *b = NULL;
	return a;
}
