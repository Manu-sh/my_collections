#undef  CLASSNAME
#define CLASSNAME lifo
#include "../ccommon.h"

STRUCT * _(new)();
void     _(free)(STRUCT *ls);
TYPENAME _(top)(const STRUCT *ls);
TYPENAME _(back)(const STRUCT *ls);
int      _(length)(const STRUCT *ls);
bool     _(isEmpty)(const STRUCT *ls);
int      _(maxSize)();

bool     _(push)(STRUCT *ls, const TYPENAME e);

/* TODO test it */
TYPENAME _(pop)(STRUCT *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
STRUCT * _(merge)(STRUCT *a, STRUCT **b);


#if 0
/* TODO */
#define ITERATOR TKPASTE(CLASSNAME, UNDERSCORE, TKPASTE(TYPEID, EMPTY, it)) /* example: vector_char_it */
#define _IT(_NAME_) TKPASTE(CLASSNAME, UNDERSCORE, TKPASTE(TYPEID, EMPTY, it_##_NAME_)) /* example: vector_char_it */

ITERATOR _IT(begin)(STRUCT *ls);
bool _IT(has_next)(ITERATOR *it);
void _IT(next)(ITERATOR *it);
void _IT(prev)(ITERATOR *it);

for (it = begin(ls); has_next(&it); next(&it)) {
	prepend(&it, "x");
	assign(&it, "x");
	append(&it, "x");
}
#endif
