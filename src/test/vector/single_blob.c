

#undef TYPENAME
#undef TYPEID

#define TYPEID char
#define TYPENAME char
#include "../../generics/gvector/gvector.h"
#include "gvector_extension.h"

#include "../../generics/gvector/gvector.c"
#include "gvector_extension.c"



#undef TYPENAME
#undef TYPEID

#define TYPEID int
#define TYPENAME int
#include "../../generics/gvector/gvector.h"
#include "gvector_extension.h"

#include "../../generics/gvector/gvector.c"
#include "gvector_extension.c"




#undef TYPENAME
#undef TYPEID

#define TYPEID void_ptr
#define TYPENAME void *
#include "../../generics/gvector/gvector.h"
#include "gvector_extension.h"

#include "../../generics/gvector/gvector.c"
#include "gvector_extension.c"



#include <stdio.h>

int main() {

	vector_char *vchar = vector_char_new();
	vector_int  *vint  = vector_int_new();

	vector_void_ptr  *vptr  = vector_void_ptr_new();

	char i;
	for (i = 'a'; i < 'z'; i++) {

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