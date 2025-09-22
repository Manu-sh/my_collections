#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../common_c99/common-c99.h"

typedef enum {
    AL_INVALID  = 0,
    AL_WORD     = sizeof(void *),     // standard alignment
    AL_DWORD    = sizeof(void *) * 2, // 16 byte SSE
    AL_QWORD    = sizeof(void *) * 4, // 32 byte avx2
    AL_EWORD    = sizeof(void *) * 8, // 64 byte alignment avx512
} posix_alignments;


typedef struct {
    uint64_t size;
    posix_alignments alignment;

    void *p; // ensure this is always the last member
} allocator_posix_align;



// a number where index is accessible and that number is multiple of align and a pow of 2
// this function is used to round_up a memory block and meet the posix_memalign() requirements
static FORCED(inline) uint64_t calc_align_index_based(uint64_t index, uint64_t align) {
    const uint64_t rounded_up = (index / align + 1) * align; // closest number multiple of align
    const uint64_t log = 64 - __builtin_clzll(rounded_up - 1); // find the smallest pow2 that can contain roundend_up
    return 2 << (log - 1);
}

static FORCED(inline) uint64_t calc_align_size_based(uint64_t byte_size, uint64_t align) {
    return calc_align_index_based(byte_size - 1, align); // -1 because real_size is not an index and doesn't require to be accessible
}


// return bytes
static FORCED(inline) uint64_t calc_mem_size(uint64_t byte_size) {
    return calc_align_size_based(byte_size, sizeof(void *));
}


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