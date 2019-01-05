#!/bin/bash

# example: ./generics-gen-vector.sh "unsigned char *" int float

for typename in "${@}"; do
typeid=${typename// /_}
typeid=${typeid//\*/p}

cat > "vector_${typeid}.h" << EOF
#undef TYPENAME
#undef TYPEID

#define TYPEID ${typeid}
#define TYPENAME ${typename}
#include "../../generics/gvector/gvector.h"
#include "../../generics/gvector/gvector.c"

#undef TYPENAME
#undef TYPEID
EOF
done
