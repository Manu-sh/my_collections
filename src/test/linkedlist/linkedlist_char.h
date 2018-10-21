#pragma once
#include <stdbool.h>

typedef struct node Node;
typedef struct linkedlist LinkedList;

// TODO provide an iterator C++ style for support an easy insertion and generic research

LinkedList * linkedlist_char_new();
void linkedlist_char_free(LinkedList *ls);

bool linkedlist_char_pushBack(LinkedList *ls, const char e);
bool linkedlist_char_pushFront(LinkedList *ls, const char e);

int  linkedlist_char_maxSize();
int  linkedlist_char_length(LinkedList *ls);
bool linkedlist_char_isEmpty(LinkedList *ls);
char linkedlist_char_front(LinkedList *ls);
char linkedlist_char_back(LinkedList *ls);
char linkedlist_char_popBack(LinkedList *ls);
char linkedlist_char_popFront(LinkedList *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
LinkedList * linkedlist_char_merge(LinkedList *a, LinkedList **b);