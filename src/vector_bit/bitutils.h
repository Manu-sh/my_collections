#pragma once

#ifdef __cplusplus
    #include <cstdint>
    #include <cassert>
    #define restrict // suppress any error since cpp doesn't have this keyword
    extern "C" {
        #include <stdbool.h> // cstdbool is deprecated in cpp17 and removed in cpp20
#else
    #include <stdint.h>
    #include <stdbool.h>
    #include <assert.h>
#endif

#include "../common_c99/common-c99.h"

#if 0
    // ceil_div(x, 8) -> same of (int)ceil(x/8.)
    static FORCED(inline) uint64_t ceil_div(uint64_t num, uint8_t div) {
        return (num-1) / div + 1;
    }
#endif

// ceil_div8(x) -> same of ceil_div(x, 8)
static FORCED(inline) uint64_t ceil_div8(uint64_t num) {
    return (num + 7) >> 3;
}

// es. 9 bit requires 2 byte -> byte_required(9) -> 2
// this function return at least 1
static FORCED(inline) uint64_t bytes_required(uint64_t bits) {
    //return (bits == 0) + ceil_div(bits, 8); // same of: return bits == 0 ? 1 : ceil_div(bits, 8);
    // return (bits == 0) + ceil_div8(bits); // same of: return bits == 0 ? 1 : ceil_div(bits, 8);
   //return !bits + ceil_div8(bits); // same of: return bits == 0 ? 1 : ceil_div(bits, 8);
   return !bits + ceil_div8(bits); // same of: return bits == 0 ? 1 : ceil_div(bits, 8);
   //return bits == 0 ? 1 : __builtin_expect(ceil_div8(bits), 1);
}

static inline void set_bit(uint8_t *restrict byte, uint8_t i) {
    assert(i < 8);
    *byte |= (1 << (7-i));
}

static FORCED(inline) void clear_bit(uint8_t *restrict byte, uint8_t i) {
    assert(i < 8);
    *byte &= ~(1 << (7-i));
}


// helper to cut away the padding bits: data >> (8 - to_extract));
// this is like the ''.substr(0, bit_length) method but for binary,
// valid ranges for bit_length is 1-8
// take_few_bits(0b11110101, 4) -> 1111
// take_few_bits(0b10110101, 4) -> 1011
static FORCED(inline) uint8_t take_few_bits(uint8_t byte, uint8_t bit_length) {
    assert(bit_length < 8);
    return byte >> (8 - bit_length);
}

static FORCED(inline) void assign_bit(uint8_t *restrict v, uint64_t bit_index, bool value) {
    const uint64_t byte_idx = bit_index >> 3; // (i/8)
    (void)(value ? set_bit(v + byte_idx, bit_index & 7) : clear_bit(v + byte_idx, bit_index & 7));  // i&7 -> i%8
}

// https://github.com/Manu-sh/huffman/blob/main/include/bitarray/BitArray.hpp#L166
static FORCED(inline) bool access_bit(const uint8_t *const restrict v, uint64_t bit_index) {
    const uint64_t byte_idx = bit_index >> 3; // (i/8)
    return (v[byte_idx] >> (7 - (bit_index&7))) & 1; // i&7 -> i%8
}


#if 0
    // a number where index is accessible and that number is multiple of align and a pow of 2
    // this function is used to round_up a memory block and meet the posix_memalign() requirements
    static FORCED(inline) uint64_t calc_align_index_based(uint64_t index, uint64_t align) {
        const uint64_t rounded_up = (index / align + 1) * align; // closest number multiple of align
        const uint64_t log = 64 - __builtin_clzll(rounded_up - 1); // find the smallest pow2 that can contain roundend_up
        return 2 << (log - 1);
    }


    // return bytes
    static FORCED(inline) uint64_t calc_mem_size(uint64_t new_bit_capacity) {
        uint64_t byte_size = bytes_required(new_bit_capacity);
        return calc_align_index_based(byte_size - 1, sizeof(void *));
    }
#endif


#ifdef __cplusplus
}
#endif
