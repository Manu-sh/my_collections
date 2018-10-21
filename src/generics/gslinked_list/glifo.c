#include "../ccommon.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node {
	struct node *next;
	TYPENAME data;
} Node;

typedef struct STRUCT {
	Node *head; /* point to a preallocated block without data */
	Node *tail;
	Node *front;
	int length; /* for obvious reasons the tail isn't count as part of length */
} STRUCT;

/* in the worst case back point to head and that coincide with tail 
(the pre-allocated block) (ls->back == ls->head == ls->tail)
so a->front it's a valid blk */

// TODO test
TYPENAME  _(top)(STRUCT *ls)  { return ls->front->data; }
TYPENAME  _(back)(STRUCT *ls)   { return ls->tail->data;  } /* there is always at least 1 blk */ 
int       _(length)(STRUCT *ls) { return ls->length;      }
bool   _(isEmpty)(STRUCT *ls)   { return !ls->head->next; }
int    _(maxSize)()             { return INT_MAX;         }

STRUCT * _(new)() {

        STRUCT *ls;
        if (!(ls = (STRUCT *)calloc(1, sizeof(STRUCT))))
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

void _(free)(STRUCT *ls) {

        if (!ls) return;

	Node *next;
	while ((next = ls->head->next)) {
                free(ls->head);
		ls->head = next;
        }

	free(ls->head), free(ls);
}

bool _(push)(STRUCT *ls, const TYPENAME e) {

        Node *blk;

        /* pre-allocation */
        if (!(blk = (Node *)calloc(1, sizeof(Node))))
                return false;

        /* save into a previous preallocated block */
        ls->head->data = (TYPENAME )e;

        /* head must point to a preallocated block without data */
	blk->next = ls->head;
	ls->front = ls->head;
        ls->head  = blk;

        ++ls->length;
        return true;
}


TYPENAME  _(pop)(STRUCT *ls) {

	/* there is always at least one block */
	assert(!_(isEmpty)(ls));
        TYPENAME ret = ls->head->next->data;
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
STRUCT * _(merge)(STRUCT *a, STRUCT **b) {

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

        /* free the container structure STRUCT (not the nodes) */
        free(*b); *b = NULL;
        return a;
}
