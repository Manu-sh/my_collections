#pragma once
#include <stdint.h>
#include <stdlib.h>

#include "align_common.h"

typedef struct __attribute__((__packed__)) {
//typedef struct {
    uint64_t requested_size;      // user requested size in bytes
    uint8_t  requested_alignment; // user requested alignment in bytes
    uint8_t  offset;              // distance (in bytes) between real_block and user_block, currently used only by malign.h
} malign_metadata;


malign_metadata * malign_meta_new(uint64_t requested_size, posix_alignment requested_alignment, uint8_t offset) {

    malign_metadata *self;

    if (UNLIKELY(!(self = (malign_metadata *)malloc(sizeof(malign_metadata)))))
        return NULL;

    self->requested_size      = requested_size;
    self->requested_alignment = (uint8_t)requested_alignment;
    self->offset              = offset;

    return self;
}

// recover the effective size of block pointed by user_pointer
// conviene passare questo valore a memcpy() visto che dovrebbe favorire la vettorizzazione
uint64_t malign_meta_user_block_aligned_size(const malign_metadata *self) {
    return align_size(self->requested_size, (posix_alignment)self->requested_alignment);
}

void malign_meta_free(malign_metadata *self) {
    free(self);
}
