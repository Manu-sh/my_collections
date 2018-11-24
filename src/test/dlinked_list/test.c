#include "dequeue_char.h"
#include <stdio.h>


int main() {

	dequeue_char *v = dequeue_char_new();

	char i;
	for (i = 'a'; i < 'z'; i++) {
		dequeue_char_pushBack(v, i);
		printf("%c added\n", dequeue_char_back(v));
	}

	dequeue_char_free(v);
	return 0;
}
