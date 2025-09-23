#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "align_common.h"

typedef struct {
    void *p;
    uint64_t size;
    posix_alignments alignment;
} allocator_posix_align;


allocator_posix_align * allocator_posix_align_new(uint64_t byte_size, posix_alignments alignment) {

    allocator_posix_align *self;

    if (!(self = (allocator_posix_align *)malloc(sizeof(allocator_posix_align))))
        return NULL;

    self->alignment = alignment;
    self->size = calc_align_size_based(byte_size, alignment);

    int ec = posix_memalign(&self->p, self->alignment, self->size);

    if (UNLIKELY(ec)) {
        free(self);
        return NULL;
    }

    self->p = __builtin_assume_aligned(self->p, self->alignment);
    return self;
}

void allocator_posix_align_free(allocator_posix_align *self) {
    free(self->p);
    free(self);
}

// mostly O(n): no cost amortization, return NULL on failure
allocator_posix_align * allocator_posix_align_resize(allocator_posix_align *self, uint64_t byte_size) {

    allocator_posix_align rollback = *self;
    rollback.p = __builtin_assume_aligned(rollback.p, rollback.alignment);

    self->size = calc_align_size_based(byte_size, self->alignment);

    if (self->size == rollback.size) // es. im asking for 768 byte but the closest number that is also a pow of 2 still 1024 which is the real size of self->p
        return self;

    int err_c = posix_memalign(&self->p, self->alignment, self->size);

    if (UNLIKELY(err_c)) {
        *self = rollback; // rollback every change
        return NULL;
    }

    self->p = __builtin_assume_aligned(self->p, self->alignment);
    memcpy(  // TODO: vettorizzazione
        __builtin_assume_aligned(self->p, self->alignment),
        __builtin_assume_aligned(rollback.p, rollback.alignment),
        rollback.size
    );

    free(rollback.p); // free the old block
    return self;
}

// O(n): no cost amortization
allocator_posix_align * allocator_posix_align_shrink(allocator_posix_align *self, uint64_t byte_size) {
    return allocator_posix_align_resize(self, byte_size);
}


// mostly O(n): return NULL on failure
allocator_posix_align * allocator_posix_align_grow(allocator_posix_align *self, uint64_t byte_size) {

    if (byte_size < self->size)
        return self;

    return allocator_posix_align_resize(self, byte_size);
}