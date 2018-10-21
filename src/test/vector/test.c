#include "vector_char.h"
#include <stdio.h>

int main() {

	Vector *v = vector_char_new();

	for (char i = 'a'; i < 'z'; i++) {
		vector_char_pushBack(v, i);
		printf("%c added\n", vector_char_back(v));
	}

	vector_char_free(v);
	return 0;
}