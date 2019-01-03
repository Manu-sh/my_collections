extern "C" {
	#include "fifo_int.h"
	#include "lifo_int.h"
}

#include <cstdio>

int main() {

	fifo_int *fifo = fifo_int_new();
	lifo_int *lifo = lifo_int_new();

	int i;
	for (i = 0; i < 10; i++) {
		fifo_int_push(fifo, i);
		lifo_int_push(lifo, i);
		printf("%d fifo top\n", fifo_int_top(fifo));
		printf("%d fifo back\n", fifo_int_back(fifo));

		printf("%d lifo top\n", lifo_int_top(lifo));
		printf("%d lifo back\n\n", lifo_int_back(lifo));
	}

	fifo_int_free(fifo);
	lifo_int_free(lifo);
	return 0;
}
