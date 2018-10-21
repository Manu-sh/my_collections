#pragma once

#ifdef FNPREFIX
	#error "shouldn't be defined"
#endif

#if !defined(TYPENAME) || !defined(FNTYPENAME)
	#error "TYPENAME and FNAME must be defined before including this file"
#endif

#define FNPREFIX dlinkedlist
#define STRUCT _(FNPREFIX)
#include "../ccommon.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

// TODO provide an iterator C++ style for support an easy insertion

typedef struct node {
	struct node *next;
	struct node *prev;
	TYPENAME data;
} Node;

typedef struct STRUCT {
	Node *head;
	Node *tail; /* point to a preallocated block without data */
	int length; /* for obvious reasons the tail isn't count as part of length */
} STRUCT;

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


int _(maxSize)() { return INT_MAX-1; }

STRUCT * _(new()) {

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

void _(free)(STRUCT *ls) {

	Node *p;

	if (!ls) return;

	for (p = ls->head; p != ls->tail;) {
	       	p = p->next;
		free(p->prev);
	}

	free(p), free(ls);
}

int  _(length)(STRUCT *ls) { 
	return ls->length;
}

bool _(isEmpty)(STRUCT *ls) { 
	// return ls->length == 0;
	return !ls->head->next; /* if there isn't another preallocated blk head point to tail and is itself the preallocated block */
}

TYPENAME _(front)(STRUCT *ls) { 
	return ls->head->data;  /* there is always at least 1 blk */
}

TYPENAME _(back)(STRUCT *ls) { 
	// return linkedlist_isEmpty(ls) ? NULL : ls->tail->prev->data;  /* prev could be NULL */

	assert(!_(isEmpty)(ls));
	return ls->tail->prev->data;
}

bool _(pushBack)(STRUCT *ls, const TYPENAME e) {

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

bool _(pushFront)(STRUCT *ls, const TYPENAME e) {

	Node *prev;

	if (!(prev = (Node *)calloc(1, sizeof(Node))))
		return false;

	prev->data = (TYPENAME)e;
	NODE_PUSH_FRONT(ls->head, prev);
	ls->head = prev;

	++ls->length;
	return true;
}

TYPENAME _(popBack)(STRUCT *ls) {

	TYPENAME ret;
	Node *tofree;

	/* check tail->prev (if tail == head list is empty and
	   heap->prev->data result in an illegal access) */
	assert(!_(isEmpty(ls));

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

TYPENAME _(popFront)(STRUCT *ls) {

	/* there is always at least one block */
	TYPENAME ret    = ls->head->data;
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

STRUCT * _(merge)(STRUCT *a, STRUCT **b) {

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

#if 0
/* linear search, return the content of the first element in case of success, otherwise NULL */
Node * find(const STRUCT *ls, bool (*eq)(void*,void*), const void *e);

void * linkedlist_removeAfterNode(STRUCT *ls, Node *a);
void * linkedlist_removeBeforeNode(STRUCT *ls, Node *a);
void * linkedlist_replaceNodeContent(Node *n, const void *e);
Node * linkedlist_insertAfterNode(STRUCT *ls, Node *a, const void *e);
Node * linkedlist_insertBeforeNode(STRUCT *ls, Node *a, const void *e);

Node * linkedlist_pushBack(STRUCT *ls, const void *e);
Node * linkedlist_pushFront(STRUCT *ls, const void *e);;
#endif

#if 0
/* remove the node between two nodes a and b and return is value */
static void * node_removeBetween(Node *a, Node *b) {
	Node *tofree = a->next;
	void *ret    = tofree->data;
	a->next = b;
	b->prev = a;
	free(tofree);
	return ret;
}

/* linear search, return the content of the first element in case of success, otherwise NULL */
Node * find(const STRUCT *ls, bool (*eq)(void*,void*), const void *e) {

	for (Node *p = ls->head; p; p = p->next)
		if (eq(p->data, (void *)e))
			return p;
	return NULL;
}

void * linkedlist_removeAfterNode(STRUCT *ls, Node *a) {

	if (!a->next || !a->next->next) 
		return NULL;

	--ls->length;
	return node_removeBetween(a, a->next->next);
}

void * linkedlist_removeBeforeNode(STRUCT *ls, Node *a) {

	if (!a->prev)
	       	return NULL;

	--ls->length;
	return a->prev->prev ? node_removeBetween(a->prev->prev, a) : node_removeBetween(a->prev, a->next);
}

void * linkedlist_replaceNodeContent(Node *n, const void *e) {
	void *ret = n->data;
	return (n->data = (void *)e), ret;
}

Node * linkedlist_insertAfterNode(STRUCT *ls, Node *a, const void *e) {

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

Node * linkedlist_insertBeforeNode(STRUCT *ls, Node *a, const void *e) { 

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

Node * linkedlist_pushBack(STRUCT *ls, const void *e) {

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

Node * linkedlist_pushFront(STRUCT *ls, const void *e) {

	Node *prev;

	if (!(prev = (Node *)calloc(1, sizeof(Node))))
		return NULL;

	prev->data = (void *)e;
	NODE_PUSH_FRONT(ls->head, prev);
	ls->head = prev;

	++ls->length;
	return ls->head;
}

#endif