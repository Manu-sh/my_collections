#include "priority_queue_int.h"
#include "priority_queue_char.h"

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

	priority_queue_int *hp;
	priority_queue_char *hpc;
	int i;

	srand(time(0));
	hp = priority_queue_int_new(lt);
	hpc = priority_queue_char_new(NULL); /* TODO */

	assert(hp);

	
	for (i = 0; i < 100; i++)
		priority_queue_int_add(hp, rand());

	printf("length: %d\n", priority_queue_int_length(hp));

	while (!priority_queue_int_isEmpty(hp))
		printf("%d\n", priority_queue_int_remove(hp));

	priority_queue_int_free(hp);
	priority_queue_char_free(hpc); /* TODO */
	return 0;
}
