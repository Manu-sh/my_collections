#include "binary_heap.h"

// change the < for different heap type
// #define MAX_HEAP
bool lt(void *a, void *b) 
{ return (*(int *)a 
#ifdef MAX_HEAP
		<
#else
		>
#endif
		*(int *)b);
}

#include <stdio.h>

// TODO implementare la funzione di bench

int main() {

	// be careful not to exceed the stack
	const int nums[] = {
		#include "nums.txt"
		,382919,48329,32189,123901,431,341091,21,2,1,3,4,5,7,8,943, 999999
	};

	Heap *hp = heap_new(lt);


	for (int i = 0; i < sizeof nums/sizeof(int); i++)
		heap_add(hp, &nums[i]);

	while(!heap_isEmpty(hp))
		heap_remove(hp);
		// printf("[%d] ==> %d\n", hp->idx, *(int*)heap_remove(hp));

	// int sa = 99999992;
	// printf("%d\n", *(int *)heap_replace(&hp, (const void*)&sa));
	// printf("%d\n", *(int *)heap_peek(&hp));

	heap_free(hp);
	return 0;

}
