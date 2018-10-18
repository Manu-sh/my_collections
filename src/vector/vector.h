#include <stdbool.h>

typedef struct vector Vector;

Vector * vector_new();
void vector_free(Vector *vct);

bool   vector_isEmpty(Vector *vct);
int    vector_length(Vector *vct);
int    vector_maxSize();
void   vector_clear(Vector *vct);
bool   vector_resize(Vector *vct, int len);
int    vector_capacity(Vector *vct);
bool   vector_shrinkToFit(Vector *vct);
void * vector_data(Vector *vct);

bool vector_pushBack(Vector *vct, void *e);

/* it dosn't check if heap is empty so you MUST check it calling vector_isEmpty() */
void * vector_popBack(Vector *vct);

// vector random access
void * vector_access(Vector *vct, int at);
void   vector_assign(Vector *vct, int at, void *e);

// get back the old element
void * vector_replace(Vector *vct, int at, void *e);