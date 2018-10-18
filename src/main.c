#include <stdio.h>
#include "singly_linked_list/fifo.h"


#if 0
int main() {

	Fifo *fifo = fifo_new();
	printf("length: %d\n", fifo_length(fifo));
	printf("empty: %d\n", fifo_isEmpty(fifo));

	fifo_push(fifo, "hello");
	// fifo_push(fifo, "world");

	printf("length: %d\n", fifo_length(fifo));
	printf("empty: %d\n", fifo_isEmpty(fifo));
	fifo_free(fifo);
	return 0;
}
#endif

#if 0
#include "doubly_linked_list/doubly_linked_list.h"

int main() {
	LinkedList *lst = linkedlist_new();

	printf("length: %d\n", linkedlist_length(lst));
	printf("empty: %d\n", linkedlist_isEmpty(lst));

	linkedlist_pushFront(lst, "hello world");
	linkedlist_pushFront(lst, "hello me");
	puts( linkedlist_popFront(lst) );

	printf("length: %d\n", linkedlist_length(lst));
	printf("empty: %d\n", linkedlist_isEmpty(lst));

	linkedlist_free(lst);
	return 0;
}
#endif

#if 0
#include "vector/vector.h"

int main() {

	Vector *v = vector_new();

	for (int i = 0; i < 10; i++) {
		vector_pushBack(v, (void *)i);
		printf("%d added\n", i);
	}

	printf("length: %d\n", vector_length(v));
	printf("capacity: %d\n", vector_capacity(v));
	printf("shrink bool: %d\n", vector_shrinkToFit(v));
	printf("capacity: %d\n", vector_capacity(v));
	
	for (int i = 0; i < 10; i++) {
		vector_pushBack(v, (void *)i);
		printf("%d added\n", i);
	}


	vector_resize(v, 0);
	printf("length: %d\n", vector_length(v));
	printf("capacity: %d\n", vector_capacity(v));

	vector_free(v);
	return 0;
}
#endif