#pragma once
#include <stdint.h>

#include "../common_c99/common-c99.h"

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
