#include "binary_heap_int.h"

// change the < for different heap type
// #define MAX_HEAP

bool lt(int a, int b) 
{
	 return a < b;
}

#include <stdio.h>
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