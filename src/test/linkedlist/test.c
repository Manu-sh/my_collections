#include "linkedlist_char.h"
#include <stdio.h>

int main() {

	LinkedList *v = linkedlist_char_new();

	for (char i = 'a'; i < 'z'; i++) {
		linkedlist_char_pushBack(v, i);
		printf("%c added\n", linkedlist_char_back(v));
	}

	linkedlist_char_free(v);
	return 0;
}