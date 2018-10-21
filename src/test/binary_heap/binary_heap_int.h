#pragma once
#include <stdbool.h>

typedef struct heap Heap;

/* INT_MAX-1 because heap start at 1 */
int heap_int_maxSize();

/* get a new heap ctx back */
Heap * heap_int_new(bool (*lt)(int,int));

/* free the memory */
void heap_int_free(Heap *hp);

/* true if there are no elements */
bool heap_int_isEmpty(const Heap *hp);

/* retrieve but not remove the root */
int heap_int_peek(const Heap *hp);

/* number of elements that heap contains */
int heap_int_length(const Heap *hp);

/* add an element */
bool heap_int_add(Heap *hp, const int e);

/* it dosn't check if heap is empty so you MUST check it calling heap_int_isEmpty() */
int heap_int_remove(Heap *hp);

/* replace the root with another element then fix the heap if is needed
return the previous root on success, NULL if an error is occurred.
More efficient than get followed by add, since only need to balance once, 
not twice, and appropriate for fixed-size heaps. */
int heap_int_replace(Heap *hp, const int e);