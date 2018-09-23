#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "lifo.h"

int main() {

	Lifo *ls  = lifo_new();
	Lifo *lst = lifo_new();

	char *pf = NULL;
	for (int i = 0; i < 10; i++) {

		asprintf(&pf, "%d", i);
		lifo_push(ls, pf);

		asprintf(&pf, "%d", i*2);
		lifo_push(lst, pf);
	}

	lifo_merge(ls, &lst);

#if 0
	int *v = malloc(10000000*sizeof(int));

	for (int i = 0; i < 10000000; i++)
		v[i] = i;


	for (int i = 0; i < 10000000; i++)
		lifo_pushBack(ls, v+i);

#endif

#if 1
	printf("%s\n", lifo_front(ls));
	printf("%s\n", lifo_back(ls));
#endif

#if 1
	char *p = lifo_pop(ls);
	while (p) {
		printf("%s\n", p);
		free(p);
		p = lifo_pop(ls);
	}

#endif

	printf("%p\n", lifo_pop(ls));
	printf("%p\n", lifo_pop(ls));
	printf("f: %p\n", lifo_front(ls));
	printf("%d\n", lifo_length(ls));


	// for (Node *p = ls->head; (p = p->next); p = p->prev)

	// puts("\nREVERSE:");
	// for (Node *p = ls->tail; p; p = p->prev)
		// printf("%s\n", p->data ? p->data : "nil");
	
	// printf("len: %d\n", ls->length);
	lifo_free(ls);
	// free(v);

	return 0;
}
