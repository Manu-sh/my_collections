#include <stdbool.h>

typedef struct vector Vector;

Vector * vector_int_new();
void vector_int_free(Vector *vct);

bool  vector_int_isEmpty(Vector *vct);
int   vector_int_length(Vector *vct);
int   vector_int_maxSize();
void  vector_int_clear(Vector *vct);
bool  vector_int_resize(Vector *vct, int len);
int   vector_int_capacity(Vector *vct);
bool  vector_int_shrinkToFit(Vector *vct);
int * vector_int_data(Vector *vct);

bool vector_int_pushBack(Vector *vct, int e);

/* it dosn't check if heap is empty so you MUST check it calling vector_int_isEmpty() */
int  vector_int_popBack(Vector *vct);

// vector random access
int  vector_int_access(Vector *vct, int at);
void   vector_int_assign(Vector *vct, int at, int e);

// get back the old element
int  vector_int_replace(Vector *vct, int at, int e);