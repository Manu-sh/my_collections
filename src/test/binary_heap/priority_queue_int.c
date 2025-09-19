#include "priority_queue_int.h"
#include "../../generics/gbinary_heap/gpriority_queue.c"


FORCED(inline) int _(capacity)(STRUCT *hp) {
	return hp->capacity;
}
