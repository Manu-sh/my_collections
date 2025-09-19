#include "../ccommon.c"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node {
	struct node *next;
	TYPENAME data;
} Node;

struct STRUCT {
	Node *head; /* point to a preallocated block without data */
	Node *tail;
	/* Node *front; front => head.next */
	int length; /* for obvious reasons the tail isn't count as part of length */
};

/* in the worst case back point to head and that coincide with tail 
(the pre-allocated block) (ls->back == ls->head == ls->tail)
so a->front it's a valid blk */

/* TODO test */
FORCED(inline) TYPENAME _(top)(const STRUCT *ls)     { return ls->head->next->data; }
FORCED(inline) TYPENAME _(back)(const STRUCT *ls)    { return ls->tail->data;  } /* there is always at least 1 blk */
FORCED(inline) int      _(length)(const STRUCT *ls)  { return ls->length;      }
FORCED(inline) bool     _(isEmpty)(const STRUCT *ls) { return !ls->head->next; }
FORCED(inline) int      _(maxSize)()                 { return INT_MAX;         }

FORCED(inline) STRUCT * _(new)() {

        STRUCT *ls;
        if (!(ls = (STRUCT *)calloc(1, sizeof(STRUCT))))
                return NULL;

        /* pre-allocation */
        if (!(ls->head = (Node *)calloc(1, sizeof(Node)))) {
                free(ls);
                return NULL;
        }

	ls->tail  = ls->head;
        return ls;
}

FORCED(inline) void _(free)(STRUCT *ls) {

        Node *next;
        if (!ls) return;

	while ((next = ls->head->next)) {
                free(ls->head);
		ls->head = next;
        }

	free(ls->head), free(ls);
}

FORCED(inline) bool _(push)(STRUCT *ls, const TYPENAME e) {

        Node *blk;

        /* pre-allocation */
        if (!(blk = (Node *)calloc(1, sizeof(Node))))
                return false;

        /* save into a previous preallocated block */
        ls->head->data = (TYPENAME )e;

        /* head must point to a preallocated block without data */
	blk->next = ls->head;
        ls->head  = blk;

        ++ls->length;
        return true;
}


FORCED(inline) TYPENAME _(pop)(STRUCT *ls) {

        TYPENAME ret;
        Node *tofree;

	/* there is always at least one block */
	assert(!_(isEmpty)(ls));
        ret = ls->head->next->data;
        tofree = ls->head;

        ls->head     = ls->head->next;
        free(tofree);

        --ls->length;
        return ret;

}



/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */

/* TODO test */
FORCED(inline) STRUCT * _(merge)(STRUCT *a, STRUCT **b) {

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

        /* remember the tail isn't count as part of length */
        a->length += (*b)->length;

        /* free the container structure STRUCT (not the nodes) */
        free(*b); *b = NULL;
        return a;
}
