/* #pragma once */
#undef  CLASSNAME
#define CLASSNAME vector
#include "../ccommon.h"

STRUCT * _(new)();
void _(free)(STRUCT *vct);

bool   _(isEmpty)(const STRUCT *vct);
int    _(length)(const STRUCT *vct);
int    _(maxSize)();
void   _(clear)(STRUCT *vct);
bool   _(resize)(STRUCT *vct, int len);
int    _(capacity)(STRUCT *vct);
bool   _(shrinkToFit)(STRUCT *vct);
TYPENAME * _(data)(const STRUCT *vct);

bool _(pushBack)(STRUCT *vct, const TYPENAME e);

/* it dosn't check if heap is empty so you MUST check it calling vector_char_isEmpty() */
TYPENAME  _(popBack)(STRUCT *vct);

/* vector random access */
TYPENAME  _(access)(const STRUCT *vct, int at);
void  _(assign)(STRUCT *vct, int at, const TYPENAME e);

/* get back the old element */
TYPENAME _(replace)(STRUCT *vct, int at, const TYPENAME e);