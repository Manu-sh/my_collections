#pragma once

#include <stdio.h>

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>
#define restrict // suppress any error since cpp doesn't have this keyword
extern "C" {
    #include <stdbool.h> // cstdbool is deprecated in cpp17 and removed in cpp20
    #include <immintrin.h>
#else
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>
#endif

#include "bitutils.h"

// C implementation of https://github.com/Manu-sh/huffman/blob/main/include/bitarray/BitArray.hpp#L34
typedef struct {
    uint64_t bit_capacity;  // how many bit can store, when bit_idx == bit_capacity-1 is full, this value should not be 0
    uint64_t bit_idx;       // point to the next available bit, which is zero when is empty
    uint8_t *v;
} vector_bit;

#define VECTOR_BIT_DEFAULT_BIT_CAPACITY 8

#if 1
    #define BLK_MALLOC(_SZ_) (malloc(_SZ_))
    #define BLK_REALLOC(_P_, _SZ_) (realloc(_P_, _SZ_))
    #define BLK_FREE(_P_) (free(_P_))
#else
    #include "../allocators/malign.h"
    #define BLK_MALLOC(_SZ_) (malign_alloc(_SZ_, AL_QWORD))
    #define BLK_REALLOC(_P_, _SZ_) (malign_realloc(_P_, _SZ_))
    #define BLK_FREE(_P_) (malign_free(_P_))
#endif

static vector_bit * vector_bit_new() {

    vector_bit *self;

    if (!(self = (vector_bit *)calloc(1, sizeof(*self))))
        return NULL;

    self->bit_capacity = VECTOR_BIT_DEFAULT_BIT_CAPACITY;
    if (!(self->v = (uint8_t *)BLK_MALLOC( bytes_required(self->bit_capacity) ))) {
        free(self);
        return NULL;
    }

    return self;
}


static void vector_bit_free(vector_bit *self) {
    if (!self) return;
    BLK_FREE(self->v);
    free(self);
}


static FORCED(inline) void vector_bit_assign(vector_bit *self, uint64_t bit_index, bool value) {
    assign_bit(self->v, bit_index, value);
}


static FORCED(inline) bool vector_bit_access(const vector_bit *self, uint64_t bit_index) {
    return access_bit(self->v, bit_index);
}


static FORCED(inline) bool vector_bit_is_empty(const vector_bit *self) {
    //return self->bit_idx == 0;
    return !self->bit_idx;
}


static FORCED(inline) void vector_bit_clear(vector_bit *self) {
    self->bit_idx = 0;
}

// number of bits stored
static FORCED(inline) uint64_t vector_bit_length(const vector_bit *self) {
    return self->bit_idx;
}

// capacity in bits (is always at least 8)
static FORCED(inline) uint64_t vector_bit_capacity(const vector_bit *self) {
    return self->bit_capacity;
}


// return true unless fail (TODO: this a bit tricky an error code would be a better opt)
static bool vector_bit_push(vector_bit *self, bool value) {

    /* doubling-halving: growUp */
    if (UNLIKELY(self->bit_idx == self->bit_capacity-1)) { // if (UNLIKELY(self->bit_idx >= self->bit_capacity-1)) {

        const uint64_t new_byte_capacity = bytes_required(self->bit_capacity * 2); // doubling the bit capacity
        uint8_t *const nv = (uint8_t *)BLK_REALLOC(self->v, new_byte_capacity);
        if (UNLIKELY(!nv)) return false;

        self->v = nv;
        self->bit_capacity = new_byte_capacity * 8;
        __builtin_prefetch(self->v,  1, 3);
    }

    vector_bit_assign(self, self->bit_idx, value), ++self->bit_idx;
    __builtin_prefetch(&self->bit_idx,  1, 3);

    return true;
}


/* it doesn't check if vector is empty so you MUST check it calling vector_bit_isempty() */
bool vector_bit_pop(vector_bit *self) {

    const bool ret = vector_bit_access(self, --self->bit_idx);

    /* doubling-halving: growDown */
    if (UNLIKELY((self->bit_capacity >> 5) > self->bit_idx)) { // if (UNLIKELY(self->bit_capacity / (4 * 8) > self->bit_idx)) {

        const uint64_t byte_capacity = bytes_required(self->bit_capacity / 2); // halving the bit capacity
        uint8_t *const nv = (uint8_t *)BLK_REALLOC(self->v, byte_capacity);
        if (UNLIKELY(!nv)) return ret; /* do nothing */

        self->v = nv;
        self->bit_capacity = byte_capacity * 8;
    }

    return ret;
}


static FORCED(inline) void vector_bit_fast_pop(vector_bit *self) {
    self->bit_idx -= !!self->bit_idx; // same of: bit_idx -= bit_idx > 0 ? 1 : 0
}


static bool vector_bit_resize(vector_bit *self, uint64_t bit_len) {

    const uint64_t byte_capacity = bytes_required(bit_len);
    uint8_t *const nv = (uint8_t *)BLK_REALLOC(self->v, byte_capacity);
    if (UNLIKELY(!nv)) return false;

    self->v = nv;
    self->bit_capacity = byte_capacity * 8;
    self->bit_idx = self->bit_idx >= bit_len ? bit_len : self->bit_idx;

    return true;
}


static bool vector_bit_shrink_to_fit(vector_bit *self) {

    const uint64_t new_byte_capacity = bytes_required(self->bit_idx+1);
    uint8_t *const nv = (uint8_t *)BLK_REALLOC(self->v, new_byte_capacity);
    if (UNLIKELY(!nv)) return false;
    self->bit_capacity = new_byte_capacity * 8;
    return true;
}


// effective byte size of the underlining vector self->v
static FORCED(inline) uint64_t vector_bit_byte_capacity(const vector_bit *self) {
    return bytes_required(self->bit_capacity);
}


static FORCED(inline) uint8_t * vector_bit_data(const vector_bit *self) {
    return self->v;
}


/* FUNCTION TO DEAL WITH UNDERLING MEMORY */
static FORCED(inline) bool vector_bit_has_padding_bits(const vector_bit *self) {
    return self->bit_idx & 7;
}

static FORCED(inline) uint64_t vector_bit_last_bit_idx(const vector_bit *self) {
    //return self->bit_idx - (self->bit_idx != 0); // same of: m_bit_idx == 0 ? m_bit_idx : m_bit_idx-1
    return self->bit_idx - !!self->bit_idx; // same of: m_bit_idx == 0 ? m_bit_idx : m_bit_idx-1
}




// how many bytes you effectively need to store the amount of bits that vector_bit actually contains
// 0 if no bits are stored
static FORCED(inline) uint64_t vector_bit_effective_byte_size(const vector_bit *self) {
    uint64_t tmp = vector_bit_length(self);
    return UNLIKELY(!tmp) ? 0 : bytes_required(tmp);
}


// last_element_byte_idx() -> accessing v[ last_element_byte_idx() ] is always valid
static FORCED(inline) uint64_t vector_bit_last_element_byte_idx(const vector_bit *self) {

    // self->bit_idx point to the next unused bit
    // if we have 8 bit stored his value is 8 but the last element is stored into this[7]

    uint64_t tmp = vector_bit_effective_byte_size(self);
    return tmp - !!tmp; // same of: effective_byte_size() == 0 ? 0 : effective_byte_size() - 1;
}

// never null, return the back byte including padding bits or a block with meaningless bits
// es. you have 14 bit stored into a v of 256 bit capacity, this function return &v[1]
// these function are useful when you have to deal with memory to make copy with memcpy etc.
static FORCED(inline) uint8_t * vector_bit_back_byte(const vector_bit *self) {
    assert(vector_bit_length(self) > 0);
    return self->v + vector_bit_last_element_byte_idx(self);
}

// how many padding bits you have, es. i have 5 bit into self and i have 3 bit of padding (obv you cannot allocate a single bit you have to allocate a byte)
static FORCED(inline) uint8_t vector_bit_padding_bits(const vector_bit *self) {
    return LIKELY(vector_bit_has_padding_bits(self)) ? 8 - (self->bit_idx & 7) : 0;
}


// cut away the padding bits from the last byte & return the last byte
static FORCED(inline) uint8_t vector_bit_back_byte_without_padding(const vector_bit *self) {
    uint8_t back_byte_with_padding_bits = *vector_bit_back_byte(self);
    return LIKELY(vector_bit_has_padding_bits(self)) ? take_few_bits(back_byte_with_padding_bits, 8-vector_bit_padding_bits(self)) : back_byte_with_padding_bits; // x&7 -> bit_length()%8
}


// the back bit, if self is empty the result is undefined
static FORCED(inline) bool vector_bit_back(const vector_bit *self) {
    return vector_bit_access(self, vector_bit_last_bit_idx(self));
}


// TODO: questa funzione va testata attentamente
static vector_bit * vector_bit_push_all(vector_bit *self, const uint8_t *src, uint64_t bit_length) {

    // uint64_t rest_bit = bit_length & 7;
    // uint64_t byte_length = (bit_length - rest_bit) >> 3;

    if (bit_length == 0)
        return self;

    // we are lucky 'cause we can block-copy
    if (UNLIKELY(!vector_bit_has_padding_bits(self))) { // if (this->bit_length() % 8 == 0)

        // if bit_length is not a multiple of 8, it's not a problem because the minimum addressable unit is 1 byte.
        // or more, so we can be sure that if bit_length were 9, there would be 2 bytes and not 1 byte.

        uint64_t sz     = vector_bit_effective_byte_size(self);
        uint64_t src_sz = bytes_required(bit_length);

        if (UNLIKELY(!vector_bit_resize(self, (sz + src_sz) * 8 ))) // 5+1 = 6
            return NULL;

        assert(src != self->v);  // non dovrebbe mai succedere, eventualmente se &o == this usare memmove()
        assert(vector_bit_byte_capacity(self) >= sz + src_sz);

        void *dst = (void *)(self->v + sz); // skip N bytes -> buf+5
        memcpy(dst, src, src_sz); // cpy(&buf[5], src, 1) -> this copy 1 byte starting from address 5 which is writeable because the size is 6

        self->bit_idx += bit_length;
        return self;
    }

    for (uint64_t i = 0; i < bit_length; ++i) {
        bool bit_value = access_bit(src, i);
        vector_bit_push(self, bit_value); // TODO: se fallisce ci faccio poco
    }

    return self;

}

static FORCED(inline) bool vector_bit_equal(const vector_bit *self, const vector_bit *other) {

    // same length
    if (vector_bit_length(self) != vector_bit_length(other))
        return false;

    // same back-byte
    if (vector_bit_back_byte_without_padding(self) != vector_bit_back_byte_without_padding(other))
        return vector_bit_length(self); // there could be 2 elements both of size 0 uninitialized

    // this value can be zero and memcpy doesn't like it, at this point we already checked that back_byte() is the same for both
    if (vector_bit_effective_byte_size(self) <= 1)
        return true;

    // we can safely use memcpy since both vector have the same length and this is a number >= 2
    return memcmp((void *)self->v, (void *)other->v, vector_bit_effective_byte_size(self) - 1) == 0; // -1 to skip the back_byte already evaluated and that could contain padding bits (garbage)
}


// deep copy
static FORCED(inline) vector_bit * vector_bit_dup(const vector_bit *self) {

    vector_bit *clone;
    if (UNLIKELY(!(clone = vector_bit_new())))
        return NULL;

    if (UNLIKELY(!vector_bit_resize(clone, self->bit_capacity)))
        return vector_bit_free(clone), (vector_bit *)NULL; // this cast if because of compiler bug

    clone->bit_idx = self->bit_idx;
    memcpy((void *)clone->v, (void *)self->v, vector_bit_effective_byte_size(self));
    return clone;
}


// just like a cpp move: move src into dst.
static FORCED(inline) void vector_bit_mov(vector_bit *dst, vector_bit *src) {

    if (dst == src) return;

    if (dst->v == src->v) {
        dst->bit_idx = src->bit_idx; // however this should never happen
        goto src_reset;
    }

    BLK_FREE(dst->v); // free the current dst buffer

    dst->v            = src->v;
    dst->bit_idx      = src->bit_idx;
    dst->bit_capacity = src->bit_capacity;


    src_reset:

    // leave the moved vector in a properly stat
    src->bit_capacity = VECTOR_BIT_DEFAULT_BIT_CAPACITY;
    src->v = (uint8_t *)BLK_MALLOC( bytes_required(src->bit_capacity) );
    src->bit_idx = 0;
    assert(src->v);
}


// return NULL on failure otherwise return self
static FORCED(inline) vector_bit * vector_bit_cat(vector_bit *self, const vector_bit *src) {
    return vector_bit_push_all(self, src->v, vector_bit_length(src));
}


// other factory methods
static vector_bit * vector_bit_make_from_cstr(const char *str) {

    vector_bit *self;
    if (!(self = vector_bit_new()))
        return NULL;

    const size_t bit_len = strlen(str);
    if (!vector_bit_resize(self, bit_len))
        goto failure;

    for (size_t i = 0; i < bit_len; ++i)
        if (!(vector_bit_push(self, !!(str[i] - '0'))))
            goto failure;

    return self;

    failure:
        vector_bit_free(self);
        return NULL;
}


static vector_bit * vector_bit_make_from_mem(const uint8_t *src, uint64_t bit_length) {

    vector_bit *self;

    if (UNLIKELY(!(self = vector_bit_new())))
        return NULL;

    if (UNLIKELY(!vector_bit_resize(self, bit_length))) {
        vector_bit_free(self);
        return NULL;
    }

    return LIKELY((uintptr_t)vector_bit_push_all(self, src, bit_length)) ? self : (vector_bit_free(self), (vector_bit *)NULL);
}


#ifdef __cplusplus
}
#endif
