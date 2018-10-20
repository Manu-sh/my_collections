#include <stdbool.h>

typedef struct vector Vector;

Vector * vector_char_new();
void vector_char_free(Vector *vct);

bool   vector_char_isEmpty(Vector *vct);
int    vector_char_length(Vector *vct);
int    vector_char_maxSize();
void   vector_char_clear(Vector *vct);
bool   vector_char_resize(Vector *vct, int len);
int    vector_char_capacity(Vector *vct);
bool   vector_char_shrinkToFit(Vector *vct);
char * vector_char_data(Vector *vct);

bool vector_char_pushBack(Vector *vct, char e);

/* it dosn't check if heap is empty so you MUST check it calling vector_char_isEmpty() */
char  vector_char_popBack(Vector *vct);

// vector random access
char  vector_char_access(Vector *vct, int at);
void  vector_char_assign(Vector *vct, int at, char e);

// get back the old element
char vector_char_replace(Vector *vct, int at, char e);