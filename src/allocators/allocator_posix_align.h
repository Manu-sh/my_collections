#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../common_c99/common-c99.h"

typedef enum {
    AL_INVALID = 0,
    AL8        = sizeof(void *),
    AL16       =  AL8  * 2,
    AL32       =  AL16 * 2,
    AL64       =  AL32 * 2,
    AL128      =  AL64 * 2,
    AL256      = AL128 * 2,
    AL512      = AL256 * 2
} posix_alignments;


typedef struct {
    void *p;
    uint64_t size;
    posix_alignments alignment;
} allocator_posix_align;



// a number where index is accessible and that number is multiple of align and a pow of 2
// this function is used to round_up a memory block and meet the posix_memalign() requirements
static FORCED(inline) uint64_t calc_align_index_based(uint64_t index, uint64_t align) {
    const uint64_t rounded_up = (index / align + 1) * align; // closest number multiple of align
    const uint64_t log = 64 - __builtin_clzll(rounded_up - 1); // find the smallest pow2 that can contain roundend_up
    return 2 << (log - 1);
}


// return bytes
static FORCED(inline) uint64_t calc_mem_size(uint64_t byte_size) {
    return calc_align_index_based(byte_size - 1, sizeof(void *));
}


allocator_posix_align allocator_posix_align_new(uint64_t byte_size, posix_alignments alignment) {

    static const allocator_posix_align invalid = {0, 0, AL_INVALID};
    allocator_posix_align data;

    data.alignment = alignment;
    data.size = calc_align_index_based(byte_size - 1, alignment);

    int ec = posix_memalign(&data.p, data.alignment, data.size);
    assert(ec == 0);

    if (UNLIKELY(ec)) return invalid;

    data.p = __builtin_assume_aligned(data.p, data.alignment);
    return data;
}



allocator_posix_align * allocator_posix_align_grow(allocator_posix_align *self, uint64_t byte_size) {

    if (byte_size < self->size)
        return self;


    allocator_posix_align ret = allocator_posix_align_new(byte_size, self->alignment);
    memcpy(__builtin_assume_aligned(ret.p, ret.alignment), __builtin_assume_aligned(self->p, self->alignment), self->size); // TODO: vettorizzazione
    free(self->p);

    *self = ret;

    return self;
}

void allocator_posix_align_free(allocator_posix_align *self) {
    free(self->p);
    memset(self, 0, sizeof(allocator_posix_align));
}
