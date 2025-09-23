#pragma once
#include "allocator_posix_align.h"
#include <stdint.h>

typedef struct  __attribute__((__packed__)) {
    uint64_t user_size;
    uint64_t user_alignment; // alignment in bytes
} malign_metadata;

malign_metadata * malign_meta_new(uint64_t user_size, posix_alignments user_alignment) {

    malign_metadata *self;

    if (UNLIKELY(!(self = (malign_metadata *)malloc(sizeof(malign_metadata)))))
        return NULL;

    self->user_size = user_size;
    self->user_alignment = user_alignment;

    return self;
}

void malign_meta_free(malign_metadata *self) {
    free(self);
}