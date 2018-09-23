#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "fifo.h"

int main() {

	Fifo *ls  = fifo_new();
	Fifo *lst = fifo_new();

	char *pf = NULL;
	for (int i = 0; i < 10000; i++) {

		asprintf(&pf, "%d", i);
		fifo_push(ls, pf);

		asprintf(&pf, "%d", i*2);
		fifo_push(lst, pf);
	}

	fifo_merge(ls, &lst);

#if 0
	int *v = malloc(10000000*sizeof(int));

	for (int i = 0; i < 10000000; i++)
		v[i] = i;


	for (int i = 0; i < 10000000; i++)
		fifo_pushBack(ls, v+i);

#endif

#if 0
	printf("%s\n", fifo_front(ls));
	printf("%s\n", fifo_back(ls));
	sleep(5);
#endif

	char *p = fifo_pop(ls);
	while (p) {
		printf("%s\n", p);
		free(p);
		p = fifo_pop(ls);
	}


	printf("%p\n", fifo_pop(ls));
	printf("%p\n", fifo_pop(ls));
	printf("%p\n", fifo_front(ls));
	printf("%d\n", fifo_length(ls));


	// for (Node *p = ls->head; (p = p->next); p = p->prev)

	// puts("\nREVERSE:");
	// for (Node *p = ls->tail; p; p = p->prev)
		// printf("%s\n", p->data ? p->data : "nil");
	
	// printf("len: %d\n", ls->length);
	fifo_free(ls);
	// free(v);

	return 0;
}
