#undef  CLASSNAME
#define CLASSNAME fifo
#include "../ccommon.h"

STRUCT * _(new)();
void     _(free)(STRUCT *ls);
TYPENAME _(back)(const STRUCT *ls);
TYPENAME _(top)(const STRUCT *ls);
int      _(length)(const STRUCT *ls);
bool     _(isEmpty)(const STRUCT *ls);
int      _(maxSize)();

bool     _(push)(STRUCT *ls, const TYPENAME e);
TYPENAME _(pop)(STRUCT *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
STRUCT * _(merge)(STRUCT *a, STRUCT **b);
