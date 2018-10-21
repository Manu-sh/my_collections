#include "fifo_int.h"
#include <stdio.h>

int main() {

	fifo_int *fifo = fifo_int_new();

	for (int i = 0; i < 10; i++) {
		fifo_int_push(fifo, i);
		printf("%d added\n", fifo_int_back(fifo));
	}

	fifo_int_free(fifo);
	return 0;
}
