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


void test_advanced_compare() {

        {
            vector_bit *bit_src = vector_bit_new();
            vector_bit_resize(bit_src, 8);

            vector_bit *bit_dst = vector_bit_new();
            vector_bit_resize(bit_dst, 8);

            vector_bit_push(bit_src, 1);
            vector_bit_push(bit_dst, 1);

            uint8_t *dst_back_byte = vector_bit_back_byte(bit_dst);
            uint8_t *src_back_byte = vector_bit_back_byte(bit_src);

            REQUIRE(access_bit(src_back_byte, 0) == 1);

            // simulate a different padding at the end between bit_dst & bit_src
            assign_bit(src_back_byte, 1, 1);
            assign_bit(dst_back_byte, 1, 0);

            REQUIRE(vector_bit_length(bit_src) == 1);
            REQUIRE(vector_bit_length(bit_dst) == 1);
            REQUIRE(*dst_back_byte != *src_back_byte);
            REQUIRE(vector_bit_equal(bit_src, bit_dst));

            vector_bit_free(bit_src);
            vector_bit_free(bit_dst);
        }

        {

            for (unsigned i = 0; i < 128; ++i) {

                vector_bit *bit_src = vector_bit_new();
                vector_bit *bit_dst = vector_bit_new();

                vector_bit_resize(bit_src, i+1);
                vector_bit_resize(bit_dst, i+1);

                //BitArray calimero(i+1);

                for (unsigned k = 1; k <= i+1; ++k) {

                    vector_bit_push(bit_src, 1);
                    vector_bit_push(bit_dst, 1);
                    //calimero.push_back(0);
                }


                REQUIRE(vector_bit_back(bit_src) == vector_bit_back(bit_dst));

                uint8_t *dst_back_byte = vector_bit_back_byte(bit_dst);
                uint8_t *src_back_byte = vector_bit_back_byte(bit_src);

                REQUIRE(vector_bit_last_element_byte_idx(bit_src) == vector_bit_last_element_byte_idx(bit_dst));
                //REQUIRE(vector_bit_back_byte_without_padding(bit_src) == vector_bit_back_byte_without_padding(bit_dst));
                REQUIRE(access_bit(src_back_byte, 0) == access_bit(dst_back_byte, 0));

                // simulate a different padding at the end between bit_dst & bit_src
                if ((i+1)%8 != 0) {
                    assign_bit(src_back_byte, (i+1)%8, 1);
                    assign_bit(dst_back_byte, (i+1)%8, 0);
                }

                // TODO:
                REQUIRE(vector_bit_length(bit_src) == i+1);
                REQUIRE(vector_bit_length(bit_dst) == i+1);

                REQUIRE(vector_bit_back(bit_src) == 1);
                REQUIRE(vector_bit_back(bit_dst) == 1);
                REQUIRE(vector_bit_back(bit_src) == vector_bit_back(bit_dst));

                // the different padding must lead different result on naive comparison
                if ((i+1)%8 != 0) {
                    REQUIRE(*dst_back_byte != *src_back_byte);
                } else {
                    REQUIRE(*dst_back_byte == *src_back_byte);
                }

                if (!vector_bit_equal(bit_src, bit_dst)) {

                    //cout << i << endl;
                    bool x = vector_bit_equal(bit_src, bit_dst);
                    if ((i+1)%8 != 0) {
                        REQUIRE(x == true);
                    } else {
                        REQUIRE(x == false);
                    }
                }

                vector_bit_free(bit_src);
                vector_bit_free(bit_dst);
            }

        }


        {
            vector_bit *a = vector_bit_make_from_cstr("001");
            vector_bit *b = vector_bit_make_from_cstr("001");
            uint8_t *a_back = vector_bit_back_byte(a);
            uint8_t *b_back = vector_bit_back_byte(b);

            // simulate different paddings
            assign_bit(a_back, 3, 0);
            assign_bit(b_back, 3, 1);

            REQUIRE(*a_back != *b_back);
            REQUIRE(vector_bit_back_byte_without_padding(a) == vector_bit_back_byte_without_padding(b));
            REQUIRE(vector_bit_equal(a, b));

            vector_bit_free(a), vector_bit_free(b);
        }



}

void test_vector_bit_dup() {
    {
        vector_bit *src = vector_bit_make_from_cstr("001");
        vector_bit *cpy = vector_bit_dup(src);

        REQUIRE(vector_bit_equal(src, cpy));
        vector_bit_free(src), vector_bit_free(cpy);
    }

    {
        vector_bit *src = vector_bit_new();
        for (unsigned i = 0; i < 200007; ++i) {
            REQUIRE(vector_bit_push(src, !(i & 1)));
            REQUIRE(vector_bit_back(src) == !(i & 1));
        }

        vector_bit *cpy = vector_bit_dup(src);
        REQUIRE(vector_bit_equal(src, cpy));
        vector_bit_free(src), vector_bit_free(cpy);
    }

}

void test_push_all() {



    {


        vector_bit *a = vector_bit_new();
        vector_bit_resize(a, 24); // 24 bit
        uint8_t *vct8 = vector_bit_data(a);

        // set 24 bit to 1
        for (unsigned i = 0; i < vector_bit_byte_capacity(a); ++i) {
            uint8_t *el = vector_bit_data(a) + i;
            for (unsigned b = 0; b < 8; ++b)
                assign_bit(el, b, 1);
        }


        { // read all 24 bits
            vector_bit *dst = vector_bit_make_from_mem(vct8, 24);

            REQUIRE(vector_bit_effective_byte_size(dst) == 3);
            REQUIRE(vector_bit_capacity(dst) == 24);
            REQUIRE(vector_bit_length(dst) == 24);
            REQUIRE(vector_bit_last_bit_idx(dst) == 23);

            for (unsigned i = 0; i < vector_bit_length(dst); ++i)
                REQUIRE(vector_bit_access(dst, i) == 1);

            vector_bit_free(dst);
        }


        { // read only 3 bits
            vector_bit *dst = vector_bit_make_from_mem(vct8, 3);

            REQUIRE(vector_bit_effective_byte_size(dst) == 1);
            REQUIRE(vector_bit_capacity(dst) == 8);
            REQUIRE(vector_bit_length(dst) == 3);
            REQUIRE(vector_bit_last_bit_idx(dst) == 2);

            for (unsigned i = 0; i < vector_bit_length(dst); ++i)
                REQUIRE(vector_bit_access(dst, i) == 1);

            vector_bit_free(dst);
        }

        vector_bit_free(a);

    }



}


// TEST_CASE("testing explicit BitArray(BitArray8 *vct, uint64_t byte_length, uint64_t bit_length)") {


int main() {

    test_vector_bit_from_cstr();
    test_concat_vector_bit();
    test_empty_vector_bit();
    test_push_vector_bit();
    test_pop_vector_bit();
    test_advanced_compare();
    test_vector_bit_dup();
    test_push_all();

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
