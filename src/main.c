#include <stdio.h>

#if 1
#include "binary_heap/binary_heap_int.h"

// change the < for different heap type
// #define MAX_HEAP

bool lt(int a, int b) 
{
	 return a < b;
}

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

	srand(time(0));

	Heap *hp = heap_int_new(lt);
	assert(hp);

	for (int i = 0; i < 100; i++)
		heap_int_add(hp, rand());

	printf("length: %d\n", heap_int_length(hp));

	while (!heap_int_isEmpty(hp))
		printf("%d\n", heap_int_remove(hp));

	heap_int_free(hp);
	return 0;
}
#endif



#if 0
#include "vector/vector_char.h"

int main() {

	Vector *v = vector_char_new();

	for (char i = 'a'; i < 'z'; i++) {
		vector_char_pushBack(v, i);
		printf("%c added\n", vector_char_back(vct));
	}

	printf("length: %d\n", vector_char_length(v));
	printf("capacity: %d\n", vector_char_capacity(v));

	printf("length: %d\n", vector_char_length(v));
	printf("capacity: %d\n", vector_char_capacity(v));

	vector_char_free(v);
	return 0;
}
#endif


#if 0
#include "vector/vector_int.h"

int main() {

	Vector *v = vector_int_new();

	for (int i = 0; i < 10; i++) {
		vector_int_pushBack(v, i);
		printf("%d added\n", vector_int_back(vct));
	}

	printf("length: %d\n", vector_int_length(v));
	printf("capacity: %d\n", vector_int_capacity(v));

	// vector_int_resize(v, 0);
	// vector_int_shrinkToFit(v);
	vector_int_clear(v);
	vector_int_shrinkToFit(v);

	printf("length: %d\n", vector_int_length(v));
	printf("capacity: %d\n", vector_int_capacity(v));

	vector_int_free(v);
	return 0;
}
#endif

#if 0
#include "singly_linked_list/fifo.h"
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
