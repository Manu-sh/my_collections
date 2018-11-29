#include <assert.h>

/* define an extra method back() that should be inlined */
__always_inline TYPENAME _(back)(const STRUCT *vct) { 
	assert(!_(isEmpty)(vct));	
	return vct->v[ _(length)(vct)-1 ];
}
