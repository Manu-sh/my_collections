#include <stdio.h>
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