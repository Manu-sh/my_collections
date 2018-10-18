#include <stdbool.h>

typedef struct node Node;
typedef struct fifo Fifo;

Fifo * fifo_new();
void   fifo_free(Fifo *ls);
void * fifo_front(Fifo *ls);
void * fifo_back(Fifo *ls);
int    fifo_length(Fifo *ls);
bool   fifo_isEmpty(Fifo *ls);
int    fifo_maxSize();

Node * fifo_push(Fifo *ls, const void *e);
void * fifo_pop(Fifo *ls);

/* merge b to a, in case of success b pointer is set to NULL and
it return the result of merge (that is a) otherwise return NULL,
you can pass null safely to this function */
Fifo * fifo_merge(Fifo *a, Fifo **b);