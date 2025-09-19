#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bitutils.h"

// C implementation of https://github.com/Manu-sh/huffman/blob/main/include/bitarray/BitArray.hpp#L34
typedef struct {
    uint64_t bit_capacity;  // how many bit can store, when bit_idx == bit_capacity-1 is full, this value should not be 0
    uint64_t bit_idx;       // point to the next available bit, which is zero when is empty
    uint8_t *v;
} vector_bit;


static vector_bit * vector_bit_new() {

    vector_bit *self;

    if (!(self = calloc(1, sizeof(*self))))
        return NULL;

    self->bit_capacity = 8;
    if (!(self->v = malloc( bytes_required(self->bit_capacity) ))) {
        free(self);
        return NULL;
    }

    return self;
}


static void vector_bit_free(vector_bit *self) {
    if (!self) return;
    free(self->v);
    free(self);
}


static FORCED(inline) void vector_bit_assign(vector_bit *self, uint64_t bit_index, bool value) {
    assign_bit(self->v, bit_index, value);
}


static FORCED(inline) bool vector_bit_access(const vector_bit *self, uint64_t bit_index) {
    return access_bit(self->v, bit_index);
}


static FORCED(inline) bool vector_bit_is_empty(const vector_bit *self) {
    return self->bit_idx == 0;
}


static FORCED(inline) void vector_bit_clear(vector_bit *self) {
    self->bit_idx = 0;
}

// number of bits stored
static FORCED(inline) uint64_t vector_bit_length(const vector_bit *self) {
    return self->bit_idx;
}

// capacity in bits (is always at least 8)
static  FORCED(inline) uint64_t vector_bit_capacity(const vector_bit *self) {
    return self->bit_capacity;
}


// return true unless fail (TODO: this a bit tricky an error code would be a better opt)
static bool vector_bit_push(vector_bit *self, bool value) {

    /* doubling-halving: growUp */
    if (self->bit_idx >= self->bit_capacity-1) {
        // puts("doubling");
        const uint64_t new_byte_capacity = bytes_required(self->bit_capacity * 2); // doubling the bit capacity
        uint8_t *const nv = realloc(self->v, new_byte_capacity);
        if (!nv) return false;

        self->v = nv;
        self->bit_capacity = new_byte_capacity * 8;
    }

    vector_bit_assign(self, self->bit_idx, value), ++self->bit_idx;
    return true;
}


/* it doesn't check if vector is empty so you MUST check it calling vector_bit_isempty() */
bool vector_bit_pop(vector_bit *self) {

    const bool ret = vector_bit_access(self, --self->bit_idx);

    /* doubling-halving: growDown */
    //if (self->bit_capacity/4 > self->bit_idx) {
    if (self->bit_capacity/16 > self->bit_idx) {
        // puts("halving");
        const uint64_t byte_capacity = bytes_required(self->bit_capacity / 2); // halving the bit capacity
        uint8_t *const nv = realloc(self->v, byte_capacity);
        if (!nv) return ret; /* do nothing */

        self->v = nv;
        self->bit_capacity = byte_capacity * 8;
    }

    return ret;
}


static bool vector_bit_resize(vector_bit *self, uint64_t bit_len) {

    const uint64_t byte_capacity = bytes_required(bit_len);
    uint8_t *const nv = realloc(self->v, byte_capacity);
    if (!nv) return false;

    self->v = nv;
    self->bit_capacity = byte_capacity * 8;
    self->bit_idx = self->bit_idx >= bit_len ? bit_len : self->bit_idx;

    return true;
}


static bool vector_bit_shrink_to_fit(vector_bit *self) {

    const uint64_t new_byte_capacity = bytes_required(self->bit_idx+1);
    uint8_t *const nv = realloc(self->v, new_byte_capacity);
    if (!nv) return false;
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



#ifdef EXTRAS

    /* FUNCTION TO DEAL WITH UNDERLING MEMORY */
    static FORCED(inline) bool vector_bit_has_padding_bits(const vector_bit *self) {
        return self->bit_idx & 7;
    }

    static FORCED(inline) uint64_t vector_bit_last_bit_idx(const vector_bit *self) {
        return self->bit_idx - (self->bit_idx != 0); // same of: m_bit_idx == 0 ? m_bit_idx : m_bit_idx-1
    }




    // how many bytes you effectively need to store the amount of bits that vector_bit actually contains
    // 0 if no bits are stored
    static FORCED(inline) uint64_t vector_bit_effective_byte_size(const vector_bit *self) {
        uint64_t tmp = vector_bit_length(self);
        return tmp == 0 ? 0 : bytes_required(tmp);
    }


    // last_element_byte_idx() -> accessing v[ last_element_byte_idx() ] is always valid
    static FORCED(inline) uint64_t vector_bit_last_element_byte_idx(const vector_bit *self) {

        // self->bit_idx point to the next unused bit
        // if we have 8 bit stored his value is 8 but the last element is stored into this[7]

        uint64_t tmp = vector_bit_effective_byte_size(self);
        return tmp - (tmp != 0); // same of: effective_byte_size() == 0 ? 0 : effective_byte_size() - 1;
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
        return vector_bit_has_padding_bits(self) ? 8 - (self->bit_idx & 7) : 0;
    }


    // cut away the padding bits from the last byte & return the last byte
    static FORCED(inline) uint8_t vector_bit_back_byte_without_padding(const vector_bit *self) {
        uint8_t back_byte_with_padding_bits = *vector_bit_back_byte(self);
        return vector_bit_has_padding_bits(self) ? take_few_bits(back_byte_with_padding_bits, 8-vector_bit_padding_bits(self)) : back_byte_with_padding_bits; // x&7 -> bit_length()%8
    }
#endif

#if 0
    // TODO: questa funzione va testata attentamente
    static bool vector_bit_push_all(vector_bit *self, const uint8_t *v, uint64_t bit_length) {

        if (bit_length == 0)
            return true;

        // we are lucky 'cause we can block-copy
        //if (this->bit_length() % 8 == 0) {
        //if (!(bit_length() & 7)) {
        if (!vector_bit_has_padding_bits(self)) {

            auto sz = vector_bit_is_empty(self) ? 0 : bytes_required(self->bit_idx);
            auto o_sz = bytes_required(bit_length);

            if (self->bit_capacity < sz + o_sz)
                if (!vector_bit_resize(self, (sz + o_sz) * 8 )) // 5+1 = 6
                    return false;

            assert(v != self->v);  // non dovrebbe mai succedere, eventualmente se &o == this usare memmove()

            void *dst = self->v + sz; // skip N bytes -> buf+5
            memcpy(dst, v, o_sz); // cpy(&buf[5], src, 1) -> this copy 1 byte starting from address 5 which is writeable because the size is 6

            self->bit_idx += bit_length;
            return true;
        }

        for (uint64_t i = 0; i < bit_length; ++i) {
            bool bit_value = access_bit(v, i);
            vector_bit_push(self, bit_value);
        }

        return true;
    }
#endif
