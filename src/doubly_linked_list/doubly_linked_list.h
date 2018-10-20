#pragma once
#include <stdbool.h>

typedef struct node Node;
typedef struct linkedlist LinkedList;

LinkedList * linkedlist_new();
void linkedlist_free(LinkedList *ls);

Node * linkedlist_pushBack(LinkedList *ls, const void *e);

Node * linkedlist_pushFront(LinkedList *ls, const void *e);

void * linkedlist_removeAfterNode(LinkedList *ls, Node *a);

void * linkedlist_removeBeforeNode(LinkedList *ls, Node *a);

void * linkedlist_replaceNodeContent(Node *n, const void *e);

Node * linkedlist_insertAfterNode(LinkedList *ls, Node *a, const void *e);

Node * linkedlist_insertBeforeNode(LinkedList *ls, Node *a, const void *e);

int linkedlist_length(LinkedList *ls);

bool linkedlist_isEmpty(LinkedList *ls);

void * linkedlist_front(LinkedList *ls);

void * linkedlist_back(LinkedList *ls);

Node * linkedlist_pushBack(LinkedList *ls, const void *e);

Node * linkedlist_pushFront(LinkedList *ls, const void *e);

void * linkedlist_popBack(LinkedList *ls);

void * linkedlist_popFront(LinkedList *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
LinkedList * linkedlist_merge(LinkedList *a, LinkedList **b);


// TODO provide an iterator C++ style for support an easy insertion and generic research

/* linear search, return the content of the first element in case of success, otherwise NULL */
Node * find(const LinkedList *ls, bool (*eq)(void*,void*), const void *e);