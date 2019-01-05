#!/bin/bash

# example: ./generics-gen-lifo.sh "unsigned char" int float

for typename in "${@}"; do
typeid=${typename// /_}
typeid=${typeid//\*/p}

cat > "lifo_${typeid}.h" << EOF
#undef TYPENAME
#undef TYPEID

#define TYPEID ${typeid}
#define TYPENAME ${typename}
#include "../../generics/gslinked_list/glifo.h"
#include "../../generics/gslinked_list/glifo.c"

#undef TYPENAME
#undef TYPEID
EOF
done
