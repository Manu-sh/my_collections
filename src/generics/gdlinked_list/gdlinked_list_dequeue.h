#undef CLASSNAME
#define CLASSNAME dlinked_list_dequeue
#include "../ccommon.h"

STRUCT * _(new());
void _(free)(STRUCT *ls);

bool _(pushBack)(STRUCT *ls, const TYPENAME e);
bool _(pushFront)(STRUCT *ls, const TYPENAME e);

int  _(length)(const STRUCT *ls);
bool _(isEmpty)(const STRUCT *ls);

TYPENAME _(front)(const STRUCT *ls);
TYPENAME _(back)(const STRUCT *ls);

TYPENAME _(popBack)(STRUCT *ls);
TYPENAME _(popFront)(STRUCT *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
STRUCT * _(merge)(STRUCT *a, STRUCT **b);
