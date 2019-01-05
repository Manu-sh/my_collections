#!/bin/bash

# example: ./generics-gen-priority-queue.sh "unsigned char *" int float

for typename in "${@}"; do
typeid=${typename// /_}
typeid=${typeid//\*/p}

cat > "priority_queue_${typeid}.h" << EOF
#undef TYPENAME
#undef TYPEID

#define TYPEID ${typeid}
#define TYPENAME ${typename}
#include "../../generics/gbinary_heap/gpriority_queue.h"
#include "../../generics/gbinary_heap/gpriority_queue.c"

#undef TYPENAME
#undef TYPEID
EOF
done
