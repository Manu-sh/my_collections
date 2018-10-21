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
	Node *head;
	Node *tail; /* point to a preallocated block without data */
	Node *back;
	int length; /* for obvious reasons the tail isn't count as part of length */
} STRUCT;

/* in the worst case back point to head and that coincide with tail 
(the pre-allocated block) (ls->back == ls->head == ls->tail)
so a->back it's a valid blk */

int       _(maxSize)()           { return INT_MAX; }
TYPENAME  _(back)(STRUCT *ls)    { return ls->back->data;  }
int       _(length)(STRUCT *ls)  { return ls->length;      }
bool      _(isEmpty)(STRUCT *ls) { return !ls->head->next; }
TYPENAME  _(top)(STRUCT *ls)   { return ls->head->data;  } /* there is always at least 1 blk */ 

STRUCT * _(new)() {

        STRUCT *ls;
        if (!(ls = (STRUCT *)calloc(1, sizeof(STRUCT))))
                return NULL;

        /* pre-allocation */
        if (!(ls->head = (Node *)calloc(1, sizeof(Node)))) {
                free(ls);
                return NULL;
        }

        ls->tail = ls->head;
	ls->back = ls->head;

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

        Node *next;

        /* pre-allocation */
        if (!(next = (Node *)calloc(1, sizeof(Node))))
                return false;

        /* save into a previous preallocated block */
        ls->tail->data = (TYPENAME)e;
        ls->tail->next = next; /* NODE_PUSH_BACK() */

        /* tail must point to a preallocated block without data */
        ls->back = ls->tail;
        ls->tail = next;

        ++ls->length;
        return true;
}


TYPENAME  _(pop)(STRUCT *ls) {

	/* there is always at least one block */
        TYPENAME ret = ls->head->data;
        Node *tofree = ls->head;

        if (!ls->head->next) { /* don't free nothing if is the last blk */
                // ls->head->data = NULL;
                ls->back       = ls->head;
                return ret;
        }

        ls->head = ls->head->next;
        free(tofree);

        --ls->length;
        return ret;

}



/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */

STRUCT * _(merge)(STRUCT *a, STRUCT **b) {

        if (!a || !b || !*b)
                return NULL;

        /* remove tail (the preallocated block without data) */
	if (a->length) free(a->tail);

	/* in the worst case back point to head and that coincide with tail 
	(the pre-allocated block that doesn't contain data)
	(ls->back == ls->head == ls->tail) so a->back it's a valid blk */

	/* merge */
	a->back->next = (*b)->head;
	a->tail       = (*b)->tail;
	a->back       = (*b)->back;

        /* remember the tail isn't count as part of length */
        a->length += (*b)->length;

        /* free the container structure Fifo (not the nodes) */
        free(*b); *b = NULL;
        return a;
}
