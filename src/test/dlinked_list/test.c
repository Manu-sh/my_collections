#include "gdlist_dequeue_char.h"
#include <stdio.h>


int main() {

	gdlist_dequeue_char *v = gdlist_dequeue_char_new();

	char i;
	for (i = 'a'; i < 'z'; i++) {
		gdlist_dequeue_char_pushBack(v, i);
		printf("%c added\n", gdlist_dequeue_char_back(v));
	}

	gdlist_dequeue_char_free(v);
	return 0;
}
