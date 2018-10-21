#include "dlinked_list_char.h"
#include <stdio.h>

int main() {

	dlinked_list_char *v = dlinked_list_char_new();

	for (char i = 'a'; i < 'z'; i++) {
		dlinked_list_char_pushBack(v, i);
		printf("%c added\n", dlinked_list_char_back(v));
	}

	dlinked_list_char_free(v);
	return 0;
}
