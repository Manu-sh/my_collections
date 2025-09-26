#include "../../vector_bit/vector_bit.h"
#include <stdio.h>

#include <assert.h>
#include <string.h>

#ifndef REQUIRE
    #define REQUIRE(_EXP_) (assert(_EXP_));
#endif

static void test_empty_vector_bit() {

    vector_bit *vct = vector_bit_new();

    REQUIRE( vector_bit_capacity(vct) >= 8 );

    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);

    vector_bit_resize(vct, 12);
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_is_empty(vct));


    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);


    vector_bit_resize(vct, 1);
    REQUIRE( vector_bit_capacity(vct) >= 8 );
    REQUIRE(vector_bit_length(vct) == 0);
    REQUIRE(vector_bit_is_empty(vct));


    REQUIRE(vector_bit_last_bit_idx(vct) == 0);
    REQUIRE(vector_bit_last_element_byte_idx(vct) == 0);
    REQUIRE(vector_bit_effective_byte_size(vct) == 0);
    REQUIRE(vector_bit_padding_bits(vct) == 0);

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

        REQUIRE(vector_bit_access(vct, vector_bit_last_bit_idx(vct)) == 1);
        REQUIRE(vector_bit_back(vct) == 1);
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

void test_vector_bit_from_cstr() {

    const char *bits = "010010101000101010100001";
    vector_bit *vb = vector_bit_make_from_cstr(bits);

    REQUIRE(strlen(bits) == vector_bit_length(vb));

    for (size_t len = vector_bit_length(vb), i = 0; i < len; ++i)
        REQUIRE(vector_bit_access(vb, i) == !!(bits[i] - '0'));

}

void test_concat_vector_bit() {

    vector_bit *a = vector_bit_new();
    vector_bit *b = vector_bit_new();

    // test empty concat
    REQUIRE(vector_bit_is_empty(a));
    REQUIRE(vector_bit_is_empty(b));
    vector_bit_push_all(a, vector_bit_data(b), vector_bit_length(b));
    REQUIRE(vector_bit_is_empty(a));


    // test concat empty + 65 bit
    for (int sz = 1; sz <= 65; ++sz) vector_bit_push(b, 1);
    vector_bit_push_all(a, vector_bit_data(b), vector_bit_length(b));
    REQUIRE(vector_bit_length(b) == 65);
    REQUIRE(vector_bit_length(a) == vector_bit_length(b));

    for (int i = 0; i < 65; ++i)
        REQUIRE(vector_bit_access(a, i) == vector_bit_access(b, i));

    REQUIRE(vector_bit_equal(a, b));

    REQUIRE(vector_bit_has_padding_bits(a));

    // 130 bit
    vector_bit_push_all(a, vector_bit_data(b), vector_bit_length(b));
    REQUIRE(vector_bit_length(a) == 130);
    REQUIRE(vector_bit_has_padding_bits(a));

    for (uint64_t i = 0; i < vector_bit_length(a); ++i) {
        //printf("%lu\n", i);
        REQUIRE(vector_bit_access(a, i) == 1);
    }


    // clear a and b
    vector_bit_clear(a);
    vector_bit_clear(b);

    const char *bits = "010010101000101010100001";
    for (size_t len = strlen(bits), i = 0; i < len; ++i)
        vector_bit_push(b, !!(bits[i] - '0'));

    REQUIRE(vector_bit_access(b, 0) == 0);
    REQUIRE(vector_bit_access(b, 1) == 1);
    REQUIRE(vector_bit_access(b, 2) == 0);
    REQUIRE(vector_bit_access(b, 3) == 0);
    REQUIRE(vector_bit_access(b, 4) == 1);
    REQUIRE(vector_bit_access(b, 5) == 0);

    REQUIRE(vector_bit_access(b, 6)  == 1);
    REQUIRE(vector_bit_access(b, 7)  == 0);
    REQUIRE(vector_bit_access(b, 8)  == 1);
    REQUIRE(vector_bit_access(b, 9)  == 0);
    REQUIRE(vector_bit_access(b, 10) == 0);
    REQUIRE(vector_bit_access(b, 11) == 0);

    REQUIRE(vector_bit_access(b, 12) == 1);
    REQUIRE(vector_bit_access(b, 13) == 0);
    REQUIRE(vector_bit_access(b, 14) == 1);
    REQUIRE(vector_bit_access(b, 15) == 0);
    REQUIRE(vector_bit_access(b, 16) == 1);
    REQUIRE(vector_bit_access(b, 17) == 0);
    REQUIRE(vector_bit_access(b, 18) == 1);

    REQUIRE(vector_bit_access(b, 19) == 0);
    REQUIRE(vector_bit_access(b, 20) == 0);
    REQUIRE(vector_bit_access(b, 21) == 0);
    REQUIRE(vector_bit_access(b, 22) == 0);
    REQUIRE(vector_bit_access(b, 23) == 1);


    vector_bit_push_all(a, vector_bit_data(b), vector_bit_length(b));
    REQUIRE(vector_bit_equal(a, b));


    // AGAIN
    vector_bit_push_all(a, vector_bit_data(b), vector_bit_length(b));

    const char *bits_bits = "010010101000101010100001010010101000101010100001";
    REQUIRE(strlen(bits_bits) == vector_bit_length(a));
    for (size_t i = 0; i < vector_bit_length(a); ++i) {
        REQUIRE(!!(bits_bits[i] - '0') == vector_bit_access(a, i));
    }

    vector_bit_free(a);
    vector_bit_free(b);


    {

        vector_bit *a = vector_bit_make_from_cstr("010");
        vector_bit *b = vector_bit_make_from_cstr("000");
        vector_bit *c = vector_bit_make_from_cstr("100");
        vector_bit *d = vector_bit_make_from_cstr("111");
        vector_bit *e = vector_bit_make_from_cstr("001");


        vector_bit *z = vector_bit_new();

        REQUIRE(vector_bit_cat(z, a));
        REQUIRE(vector_bit_equal(z, a));


        // TODO: ok let's leave some memory leak here it's just a test, vector_bit_make_from_cstr() should be vector_bit_free()
        REQUIRE(vector_bit_equal(vector_bit_cat(z, b), vector_bit_make_from_cstr("010000")));
        REQUIRE(vector_bit_equal(vector_bit_cat(z, c), vector_bit_make_from_cstr("010000100")));
        REQUIRE(vector_bit_equal(vector_bit_cat(z, d), vector_bit_make_from_cstr("010000100111")));
        REQUIRE(vector_bit_equal(vector_bit_cat(z, e), vector_bit_make_from_cstr("010000100111001")));


        vector_bit_free(a);
        vector_bit_free(b);
        vector_bit_free(c);
        vector_bit_free(d);
        vector_bit_free(e);

        vector_bit_free(z);
    }
}

int main() {

    test_vector_bit_from_cstr();
    test_concat_vector_bit();
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
