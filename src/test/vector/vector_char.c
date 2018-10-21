#include "vector_char.h"

#define TYPE char
#define FNTYPE _char_

#include "../../generics/gvector/gvector.h"

/* define an extra method back() that should be inlined */
__always_inline TYPE _(back)(Vector *vct) { return vct->v[ _(length)(vct)-1 ]; }