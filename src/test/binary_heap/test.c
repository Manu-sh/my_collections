#include "binary_heap_int.h"
#include "binary_heap_char.h"

/* change the < for different heap type */
/* #define MAX_HEAP */

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

	binary_heap_int *hp;
	binary_heap_char *hpc;
	int i;

	srand(time(0));
	hp = binary_heap_int_new(lt);
	hpc = binary_heap_char_new(NULL); /* TODO */

	assert(hp);

	
	for (i = 0; i < 100; i++)
		binary_heap_int_add(hp, rand());

	printf("length: %d\n", binary_heap_int_length(hp));

	while (!binary_heap_int_isEmpty(hp))
		printf("%d\n", binary_heap_int_remove(hp));

	binary_heap_int_free(hp);
	binary_heap_char_free(hpc); /* TODO */
	return 0;
}