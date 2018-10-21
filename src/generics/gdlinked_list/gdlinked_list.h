// #pragma once
#define CLASSNAME dlinked_list
#include "../hcommon.h"

STRUCT * _(new());
void _(free)(STRUCT *ls);

bool _(pushBack)(STRUCT *ls, const TYPENAME e);
bool _(pushFront)(STRUCT *ls, const TYPENAME e);

int  _(length)(STRUCT *ls);
bool _(isEmpty)(STRUCT *ls);

TYPENAME _(front)(STRUCT *ls);
TYPENAME _(back)(STRUCT *ls);

TYPENAME _(popBack)(STRUCT *ls);
TYPENAME _(popFront)(STRUCT *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
STRUCT * _(merge)(STRUCT *a, STRUCT **b);