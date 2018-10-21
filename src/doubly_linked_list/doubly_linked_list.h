#pragma once
#include <stdbool.h>

typedef struct node Node;
typedef struct linkedlist LinkedList;

// TODO provide an iterator C++ style for support an easy insertion and generic research

LinkedList * linkedlist_new();
void linkedlist_free(LinkedList *ls);

bool linkedlist_pushBack(LinkedList *ls, const void *e);
bool linkedlist_pushFront(LinkedList *ls, const void *e);

int    linkedlist_maxSize();
int    linkedlist_length(LinkedList *ls);
bool   linkedlist_isEmpty(LinkedList *ls);
void * linkedlist_front(LinkedList *ls);
void * linkedlist_back(LinkedList *ls);
void * linkedlist_popBack(LinkedList *ls);
void * linkedlist_popFront(LinkedList *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
LinkedList * linkedlist_merge(LinkedList *a, LinkedList **b);