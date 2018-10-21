#undef CLASSNAME
#define CLASSNAME binary_heap

#include "../hcommon.h"

int _(maxSize)();

/* get a new heap ctx back */
STRUCT * _(new)(bool (*lt)(TYPENAME,TYPENAME));

/* free the memory */
void _(free)(STRUCT *hp);

/* true if there are no elements */
bool _(isEmpty)(const STRUCT *hp);

/* retrieve but not remove the root */
TYPENAME _(peek)(const STRUCT *hp);

/* number of elements that heap contains */
int _(length)(const STRUCT *hp);

/* add an element */
bool _(add)(STRUCT *hp, const TYPENAME e);

/* it dosn't check if heap is empty so you MUST check it calling _(isEmpty()) */
TYPENAME _(remove)(STRUCT *hp);

/* replace the root with another element then fix the heap if is needed
return the previous root on success, NULL if an error is occurred.
More efficient than get followed by add, since only need to balance once, 
not twice, and appropriate for fixed-size heaps. */
TYPENAME _(replace)(STRUCT *hp, const TYPENAME e);