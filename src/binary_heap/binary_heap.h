#pragma once
#include <stdbool.h>

typedef struct heap Heap;

/* INT_MAX-1 because heap start at 1 */
int heap_maxSize();

/* get a new heap ctx back */
Heap * heap_new(bool (*lt)(void*,void*));

/* free the memory */
void heap_free(Heap *hp);

/* true if there are no elements */
bool heap_isEmpty(const Heap *hp);

/* retrieve but not remove the root */
void * heap_peek(const Heap *hp);

/* number of elements that heap contains */
int heap_length(const Heap *hp);

/* add an element */
bool heap_add(Heap *hp, const void *e);

/* it dosn't check if heap is empty so you MUST check it calling heap_isEmpty() */
void * heap_remove(Heap *hp);

/* replace the root with another element then fix the heap if is needed
return the previous root on success, NULL if an error is occurred.
More efficient than get followed by add, since only need to balance once, 
not twice, and appropriate for fixed-size heaps. */
void * heap_replace(Heap *hp, const void *e);