#pragma once
#include <stdint.h>
#include <stdlib.h>

#include "align_common.h"

typedef struct  __attribute__((__packed__)) {
    uint64_t user_size;      // size in bytes
    uint8_t  user_alignment; // alignment in bytes
    uint8_t  offset;         // distance (in bytes) between real_block and user_block, currently used only by malign.h
} malign_metadata;

malign_metadata * malign_meta_new(uint64_t user_size, posix_alignments user_alignment, uint8_t offset) {

    malign_metadata *self;

    if (UNLIKELY(!(self = (malign_metadata *)malloc(sizeof(malign_metadata)))))
        return NULL;

    self->user_size      = user_size;
    self->user_alignment = (uint8_t)user_alignment;
    self->offset         = offset;

    return self;
}

void malign_meta_free(malign_metadata *self) {
    free(self);
}
