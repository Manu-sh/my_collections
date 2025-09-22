#ifndef EXTRAS
    #define EXTRAS
#endif

#include "../../vector_bit/vector_bit.h"
#include <stdio.h>

#include <assert.h>

#ifndef REQUIRE
    #define REQUIRE(_EXP_) (assert(_EXP_));
#endif

static void test_empty_vector_bit() {

    vector_bit *vct = vector_bit_new();

    REQUIRE( vector_bit_capacity(vct) >= 8 );

    #ifdef EXTRAS
        REQUIRE(vector_bit_last_bit_idx(vct) == 0);
        REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
        REQUIRE(vector_bit_effective_byte_size(vct) == 0);
        REQUIRE(vector_bit_padding_bits(vct) == 0);
    #endif

        vector_bit_resize(vct, 12);
        REQUIRE(vector_bit_length(vct) == 0);
        REQUIRE(vector_bit_is_empty(vct));

    #ifdef EXTRAS
        REQUIRE(vector_bit_last_bit_idx(vct) == 0);
        REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
        REQUIRE(vector_bit_effective_byte_size(vct) == 0);
        REQUIRE(vector_bit_padding_bits(vct) == 0);
    #endif

        vector_bit_resize(vct, 1);
        REQUIRE( vector_bit_capacity(vct) >= 8 );
        REQUIRE(vector_bit_length(vct) == 0);
        REQUIRE(vector_bit_is_empty(vct));

    #ifdef EXTRAS
        REQUIRE(vector_bit_last_bit_idx(vct) == 0);
        REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
        REQUIRE(vector_bit_effective_byte_size(vct) == 0);
        REQUIRE(vector_bit_padding_bits(vct) == 0);
    #endif

    vector_bit_free(vct);

}


static void test_push_vector_bit() {

    vector_bit *vct = vector_bit_new();

    for (uint64_t i = 1; i <= 7; ++i) {  // no realloc test
        vector_bit_push(vct, 1);
        REQUIRE(!vector_bit_is_empty(vct));

        REQUIRE(vector_bit_last_bit_idx(vct) == i-1);
        REQUIRE(vector_bit_length(vct) == i);

        REQUIRE(vector_bit_capacity(vct) == 8);
        //printf("%d\n", vector_bit_last_element_byte_idx(vct));
        REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
        REQUIRE(vector_bit_effective_byte_size(vct) == 1);
        REQUIRE(vector_bit_padding_bits(vct) == 8-i);
    }


    // realloc test, inserting the 8th bit
    vector_bit_push(vct, 1);

    REQUIRE(!vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 7);
    REQUIRE(vector_bit_length(vct) == 8);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);

    REQUIRE(vector_bit_capacity(vct) >= 16);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 0);


    // 9 bit
    vector_bit_push(vct, 1);

    REQUIRE(!vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 8);
    REQUIRE(vector_bit_length(vct) == 9);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 1);

    REQUIRE(vector_bit_capacity(vct) >= 16);
    REQUIRE(vector_bit_effective_byte_size(vct) == 2);
    REQUIRE(vector_bit_padding_bits(vct) == 7);

    REQUIRE(vector_bit_access(vct, 8) == 1);


    vector_bit_free(vct);

}


static void test_pop_vector_bit() {

    vector_bit *vct = vector_bit_new();


    vector_bit_push(vct, 0); // PUSH 0
    REQUIRE(vector_bit_access(vct, 0) == 0);
    REQUIRE(vector_bit_pop(vct) == 0); // remove


    REQUIRE(vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_capacity(vct) >= 8);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_byte_capacity(vct) >= 1);
    REQUIRE(vector_bit_padding_bits(vct) == 0);


    vector_bit_push(vct, 1); // PUSH 1
    REQUIRE(vector_bit_access(vct, 0) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_length(vct) == 1);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_capacity(vct) >= 8);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 7);

    vector_bit_clear(vct);

    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_capacity(vct) >= 8);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);


    vector_bit_push(vct, 1); // PUSH 8 bit
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);
    vector_bit_push(vct, 1);


    for (uint64_t i = 0; i < 8; ++i)
        REQUIRE(vector_bit_access(vct, i) == 1);


    // push 9th bit
    vector_bit_push(vct, 1);

    REQUIRE(vector_bit_access(vct, 8) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 8);
    REQUIRE(vector_bit_length(vct) == 9);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 1);
    REQUIRE(vector_bit_capacity(vct) >= 16);
    REQUIRE(vector_bit_effective_byte_size(vct) == 2);
    REQUIRE(vector_bit_padding_bits(vct) == 7);


    // start remove every bit inserted
    REQUIRE(vector_bit_pop(vct) == 1); // remove 9th bit
    REQUIRE(vector_bit_last_bit_idx(vct) == 7)
    REQUIRE(vector_bit_length(vct) == 8);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 0);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 6)
    REQUIRE(vector_bit_length(vct) == 7);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 1);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 5)
    REQUIRE(vector_bit_length(vct) == 6);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 2);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 4)
    REQUIRE(vector_bit_length(vct) == 5);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 3);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 3)
    REQUIRE(vector_bit_length(vct) == 4);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 4);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 2)
    REQUIRE(vector_bit_length(vct) == 3);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 5);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 1)
    REQUIRE(vector_bit_length(vct) == 2);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 6);


    REQUIRE(vector_bit_pop(vct) == 1);
    REQUIRE(vector_bit_last_bit_idx(vct) == 0)
    REQUIRE(vector_bit_length(vct) == 1);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 1);
    REQUIRE(vector_bit_padding_bits(vct) == 7);


    REQUIRE(vector_bit_pop(vct) == 1); // remove the last bit
    REQUIRE(vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 0)
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);

    vector_bit_free(vct);



    vct = vector_bit_new();
    vector_bit_resize(vct, 9); // reserve space for 9 bit

    for (uint64_t i = 0; i < 9; ++i)
        REQUIRE(vector_bit_push(vct, 1));


    REQUIRE(!vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 8)
    REQUIRE(vector_bit_length(vct) == 9);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 1);
    REQUIRE(vector_bit_effective_byte_size(vct) == 2);
    REQUIRE(vector_bit_padding_bits(vct) == 7);
    REQUIRE(vector_bit_capacity(vct) >= 16);

    // remove all elements except the last one
    for (uint64_t i = vector_bit_length(vct); i > 1; --i) {
        REQUIRE(!vector_bit_is_empty(vct));
        REQUIRE(vector_bit_last_bit_idx(vct) == (i-1));
        REQUIRE(vector_bit_length(vct) == (i-1)+1);

        REQUIRE(vector_bit_last_element_byte_idx(vct) == ((i-1) == 8 ? 1 : 0));
        REQUIRE(vector_bit_pop(vct) == 1);
    }


    REQUIRE(vector_bit_pop(vct) == 1); // remove the last bit
    REQUIRE(vector_bit_is_empty(vct));
    REQUIRE(vector_bit_last_bit_idx(vct) == 0)
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);

    vector_bit_free(vct);
}


int main() {

    test_empty_vector_bit();
    test_push_vector_bit();
    test_pop_vector_bit();

    vector_bit *vct = vector_bit_new();
    for (int i = 0; i < 145; ++i) {
        vector_bit_push(vct, !(i & 1));
        printf("bit[%d] = %d\n", i, vector_bit_access(vct, i));
    }

    assert(vector_bit_length(vct) == 145);
    printf("capacity %zu\n", vector_bit_capacity(vct));
    //assert(vector_bit_capacity(vct) == 256); // TODO: troppo spazio?

    int times = 0;
    while (!vector_bit_is_empty(vct)) {
        printf("step %d -> %d\n", ++times, vector_bit_pop(vct));
    }

    assert(vector_bit_length(vct) == 0);
    printf("capacity %zu\n", vector_bit_capacity(vct));
    assert(vector_bit_capacity(vct) >= 8);

    // test an invalid pop()
    vector_bit_fast_pop(vct);
    assert(vector_bit_length(vct) == 0);

    vector_bit_free(vct);
    return 0;
}