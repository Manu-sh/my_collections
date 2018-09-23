#include "vector.h"

#include <stdio.h>
int main() {

	Vector *v = vector_new();

	for (int i = 0; i < 10; i++) {
		vector_pushBack(v, (void *)i);
		printf("%d added\n", i);
	}

	printf("=> %d\n", (int)vector_replace(v, 3, 3000));
	printf("length: %d\n", vector_length(v));

	while (!vector_isEmpty(v))
		printf("%d\n", (int)vector_popBack(v));

	// for (int i = 0; i < vector_length(v); i++)
	//	printf("%d\n", (int)vector_access(v, i));

	vector_free(v);
	return 0;
}
