#include "fifo.h"

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct node {
	struct node *next;
	void *data;
} Node;

typedef struct fifo {
	Node *head;
	Node *tail; /* point to a preallocated block without data */
	Node *back;
	int length; /* for obvious reasons the tail isn't count as part of length */
} Fifo;

/* in the worst case back point to head and that coincide with tail 
(the pre-allocated block) (ls->back == ls->head == ls->tail)
so a->back it's a valid blk */

int    fifo_maxSize()         { return INT_MAX; }
void * fifo_back(Fifo *ls)    { return ls->back->data;  }
int    fifo_length(Fifo *ls)  { return ls->length;      }

/* TODO avoid to use length */
// bool   fifo_isEmpty(Fifo *ls) { return ls->length == 0; }

bool   fifo_isEmpty(Fifo *ls) { return !ls->head->next; }
void * fifo_front(Fifo *ls)   { return ls->head->data;  } /* there is always at least 1 blk */ 

Fifo * fifo_new() {

        Fifo *ls;
        if (!(ls = (Fifo *)calloc(1, sizeof(Fifo))))
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

void fifo_free(Fifo *ls) {

        if (!ls) return;

	Node *next;
	while ((next = ls->head->next)) {
                free(ls->head);
		ls->head = next;
        }

	free(ls->head), free(ls);
}

Node * fifo_push(Fifo *ls, const void *e) {

        Node *next, *ret = ls->tail;

        /* pre-allocation */
        if (!(next = (Node *)calloc(1, sizeof(Node))))
                return NULL;

        /* save into a previous preallocated block */
        ls->tail->data = (void *)e;
	ls->tail->next = next; /* NODE_PUSH_BACK() */

        /* tail must point to a preallocated block without data */
	ls->back = ls->tail;
        ls->tail = next;

        ++ls->length;
        return ret;
}


void * fifo_pop(Fifo *ls) {

	/* there is always at least one block */
        void *ret    = ls->head->data;
        Node *tofree = ls->head;

        if (!ls->head->next) { /* don't free nothing if is the last blk */
                ls->head->data = NULL;
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

Fifo * fifo_merge(Fifo *a, Fifo **b) {

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
