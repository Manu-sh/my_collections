#pragma once
#include <stdint.h>

#include "../common_c99/common-c99.h"

// log2 for n which must be a pow of 2
static FORCED(inline) uint64_t log2fast(uint64_t n_pow_of_2) {
    //return 63 - __builtin_clzll(n_pow_of_2);
    return 64 - __builtin_clzll(n_pow_of_2 - !!n_pow_of_2); // questa versione è più tollerante nel caso in cui n_pow_of_2 dovesse essere 0
}

typedef enum {
    AL_INVALID  = 0,
    AL_WORD     = sizeof(void *),     // standard alignment
    AL_DWORD    = sizeof(void *) * 2, // 16 byte SSE
    AL_QWORD    = sizeof(void *) * 4, // 32 byte avx2
    AL_EWORD    = sizeof(void *) * 8, // 64 byte alignment avx512
} posix_alignments;


static FORCED(inline) uint64_t round_up_to_word(uint64_t size, posix_alignments align) {

    assert(size && align);

    uint64_t index = size - 1; // TODO: cambiare in size - !!size
    const uint64_t log2 = log2fast(align);
    //printf("log2(%d)=%lu\n", align, log2);
    return ((index >> log2) + 1) << log2; // same of: return (index / align + 1) * align; when align is a pow of 2
}


// const uint64_t rounded_up = (index / align + 1) * align;
static FORCED(inline) uint64_t round_up(uint64_t size, uint64_t align) {
     uint64_t index = size - 1; // TODO: cambiare in size - !!size
     return (index / align + 1) * align;
}

// a number where index is accessible and that number is multiple of align and a pow of 2
// this function is used to round_up a memory block and meet the posix_memalign() requirements
static FORCED(inline) uint64_t calc_align_index_based(uint64_t index, uint64_t align) {
    const uint64_t rounded_up = (index / align + 1) * align; // closest number multiple of align
    const uint64_t log = 64 - __builtin_clzll(rounded_up - 1); // find the smallest pow2 that can contain roundend_up
    return 2 << (log - 1);
}

// TODO: bug con size 0?
static FORCED(inline) uint64_t calc_align_size_based(uint64_t byte_size, uint64_t align) {
    return calc_align_index_based(byte_size - 1, align); // -1 because real_size is not an index and doesn't require to be accessible
}

// return bytes
static FORCED(inline) uint64_t calc_mem_size(uint64_t byte_size) {
    return calc_align_size_based(byte_size, sizeof(void *));
}
