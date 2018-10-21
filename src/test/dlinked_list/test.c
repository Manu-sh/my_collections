#include "dlinked_list_dequeue_char.h"
#include <stdio.h>

int main() {

	dlinked_list_dequeue_char *v = dlinked_list_dequeue_char_new();

	char i;
	for (i = 'a'; i < 'z'; i++) {
		dlinked_list_dequeue_char_pushBack(v, i);
		printf("%c added\n", dlinked_list_dequeue_char_back(v));
	}

	dlinked_list_dequeue_char_free(v);
	return 0;
}
