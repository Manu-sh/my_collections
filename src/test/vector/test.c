#include "vector_char.h"
#include "vector_int.h"
#include "vector_void_ptr.h"

#include <stdio.h>

int main() {

	vector_char *vchar = vector_char_new();
	vector_int  *vint  = vector_int_new();

	vector_void_ptr  *vptr  = vector_void_ptr_new();

	for (char i = 'a'; i < 'z'; i++) {

		vector_char_pushBack(vchar, i);
		vector_int_pushBack(vint, i);

		printf("%c added\n", vector_char_back(vchar));
		printf("%d added\n", vector_int_back(vint));
	}

	vector_char_free(vchar);
	vector_int_free(vint);
	vector_void_ptr_free(vptr);
	return 0;
}
