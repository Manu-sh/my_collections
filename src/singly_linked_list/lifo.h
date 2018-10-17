#include <stdbool.h>

typedef struct node Node;
typedef struct lifo Lifo;

Lifo * lifo_new();
void lifo_free(Lifo *ls);

void * lifo_front(Lifo *ls);
void * lifo_back(Lifo *ls);
int    lifo_length(Lifo *ls);
bool   lifo_isEmpty(Lifo *ls);

Node * lifo_push(Lifo *ls, const void *e);
void * lifo_pop(Lifo *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
Lifo * lifo_merge(Lifo *a, Lifo **b);