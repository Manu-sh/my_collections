#include "vector_char.h"
#include "vector_int.h"
#include "vector_void_ptr.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

int main() {

	char i;

	vector_char *vchar = vector_char_new();
	vector_int  *vint  = vector_int_new();
	vector_void_ptr  *vptr  = vector_void_ptr_new();


	assert(vector_char_length(vchar) == 0);
	assert(vector_char_capacity(vchar) == 10);

	vector_char_pushBackAll(vchar, "hello world", strlen("hello world"));
	
	printf("%d\n", vector_char_length(vchar));
	printf("%d\n", vector_char_capacity(vchar));

	assert(vector_char_length(vchar) == strlen("hello world"));
	assert((unsigned)vector_char_capacity(vchar) > strlen("hello world"));

	vector_char_pushBack(vchar, '\0');

	assert(strcmp((void *)vector_char_data(vchar), "hello world") == 0);

#if 1
	
	for (i = 'a'; i < 'z'; i++) {

		vector_char_pushBack(vchar, i);
		vector_int_pushBack(vint, i);

		printf("%c added\n", vector_char_back(vchar));
		printf("%d added\n", vector_int_back(vint));
	}
#endif

	vector_char_free(vchar);
	vector_int_free(vint);
	vector_void_ptr_free(vptr);
	return 0;
}