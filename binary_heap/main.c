#include "binary_heap.h"

/* change the < for different heap type */

// #define MAX_HEAP
bool lt(void *a, void *b) {

	return (*(int *)a 

		#ifdef MAX_HEAP
			<
		#else
			>
		#endif

	*(int *)b);
}


#include <stdio.h>

int main() {

	Heap *hp = heap_new(lt);


	heap_free(hp);
	return 0;
}
