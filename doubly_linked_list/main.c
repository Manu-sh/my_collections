#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "doubly_linked_list.h"

bool eq(void *a, void *b) { return strcmp(a,b) == 0; }

int main() {

	LinkedList *ls  = linkedlist_new();
	LinkedList *lst = linkedlist_new();

	char *pf = NULL;
	for (int i = 0; i < 10000; i++) {

		asprintf(&pf, "%d", i);
		linkedlist_pushBack(ls, pf);

		asprintf(&pf, "%d", i*2);
		linkedlist_pushBack(lst, pf);

	}

	linkedlist_merge(ls, &lst);

#if 0
	int *v = malloc(10000000*sizeof(int));

	for (int i = 0; i < 10000000; i++)
		v[i] = i;


	for (int i = 0; i < 10000000; i++)
		linkedlist_pushBack(ls, v+i);

#endif

#if 0
	printf("%s\n", linkedlist_front(ls));
	printf("%s\n", linkedlist_back(ls));
	sleep(5);
#endif

	char *p = linkedlist_popFront(ls);
	while (p) {
		printf("%s\n", p);
		free(p);
		p = linkedlist_popFront(ls);
	}



	// for (Node *p = ls->head; (p = p->next); p = p->prev)

	// puts("\nREVERSE:");
	// for (Node *p = ls->tail; p; p = p->prev)
		// printf("%s\n", p->data ? p->data : "nil");
	
	// printf("len: %d\n", ls->length);
	linkedlist_free(ls);
	// free(v);

	return 0;
}
