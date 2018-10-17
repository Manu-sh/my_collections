#include "lifo.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
	struct node *next;
	void *data;
} Node;

typedef struct lifo {
	Node *head; /* point to a preallocated block without data */
	Node *tail;
	Node *front;
	int length; /* for obvious reasons the tail isn't count as part of length */
} Lifo;

/* in the worst case back point to head and that coincide with tail 
(the pre-allocated block) (ls->back == ls->head == ls->tail)
so a->front it's a valid blk */

// TODO test
void * lifo_front(Lifo *ls)   { return ls->front->data; }
void * lifo_back(Lifo *ls)    { return ls->tail->data;  } /* there is always at least 1 blk */ 
int    lifo_length(Lifo *ls)  { return ls->length;      }
bool   lifo_isEmpty(Lifo *ls) { return ls->length == 0; }

Lifo * lifo_new() {

        Lifo *ls;
        if (!(ls = (Lifo *)calloc(1, sizeof(Lifo))))
                return NULL;

        /* pre-allocation */
        if (!(ls->head = (Node *)calloc(1, sizeof(Node)))) {
                free(ls);
                return NULL;
        }

	ls->tail  = ls->head;
	ls->front = ls->head;

        return ls;
}

void lifo_free(Lifo *ls) {

        if (!ls) return;

	Node *next;
	while ((next = ls->head->next)) {
                free(ls->head);
		ls->head = next;
        }

	free(ls->head), free(ls);
}

Node * lifo_push(Lifo *ls, const void *e) {

        Node *blk, *ret = ls->head;

        /* pre-allocation */
        if (!(blk = (Node *)calloc(1, sizeof(Node))))
                return NULL;

        /* save into a previous preallocated block */
        ls->head->data = (void *)e;

        /* head must point to a preallocated block without data */
	blk->next = ls->head;
	ls->front = ls->head;
        ls->head  = blk;

        ++ls->length;
        return ret;
}


void * lifo_pop(Lifo *ls) {

	/* there is always at least one block */
	if (lifo_isEmpty(ls)) {
                ls->head->data = NULL;
		ls->tail       = ls->head;
		ls->front      = ls->head;
		return NULL;
	}

        void *ret    = ls->head->next->data;
        Node *tofree = ls->head;

        ls->head     = ls->head->next;
	ls->front    = ls->head->next;
        free(tofree);

        --ls->length;
        return ret;

}



/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */

// TODO test
Lifo * lifo_merge(Lifo *a, Lifo **b) {

	Node *n;

        if (!a || !b || !*b)
                return NULL;

        /* remove head (the preallocated block without data) */
	n = a->head->next;

	if (a->length)
		free(a->head);

	/* merge */
	(*b)->tail->next = n;
	a->head    = (*b)->head;
	a->tail    = (*b)->tail;
	a->front   = (*b)->front;

        /* remember the tail isn't count as part of length */
        a->length += (*b)->length;

        /* free the container structure Lifo (not the nodes) */
        free(*b); *b = NULL;
        return a;
}