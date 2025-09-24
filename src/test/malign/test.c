#ifndef EXTRAS
    #define EXTRAS
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifndef REQUIRE
    #define REQUIRE(_EXP_) (assert(_EXP_));
#endif

#define DEBUG
#include "../../allocators/malign.h"

void test_round_up_to_word() {

    //printf("%lu\n", log2fast(0));

    // sizeof(void *) alignment
    for (int sz = 1; sz <= 8; ++sz)
        REQUIRE(round_up_to_word(sz, AL_WORD) == 8);

    for (int sz = 8+1; sz <= 16; ++sz)
        REQUIRE(round_up_to_word(sz, AL_WORD) == 16);

    for (int sz = 16+1; sz <= 24; ++sz)
        REQUIRE(round_up_to_word(sz, AL_WORD) == 24);


    // sizeof(void *) * 2 alignment
    for (int sz = 1; sz <= 8; ++sz)
        REQUIRE(round_up_to_word(sz, AL_DWORD) == 16);

    for (int sz = 8+1; sz <= 16; ++sz)
        REQUIRE(round_up_to_word(sz, AL_DWORD) == 16);

    for (int sz = 16+1; sz <= 24; ++sz)
        REQUIRE(round_up_to_word(sz, AL_DWORD) == 32);



    REQUIRE(round_up_to_word(113, AL_WORD) == 120);
    REQUIRE(round_up_to_word(256000000, AL_WORD) == 256000000);

    REQUIRE(round_up_to_word(113, AL_DWORD) == 128);
    REQUIRE(round_up_to_word(113, AL_DWORD) == 128);

    REQUIRE(round_up_to_word(113, AL_QWORD) == 128);
    REQUIRE(round_up_to_word(256000000, AL_QWORD) == 256000000);

    REQUIRE(round_up_to_word(113, AL_EWORD) == 128);
    REQUIRE(round_up_to_word(256000000, AL_EWORD) == 256000000);

}


/*
    la potenza di 2 è il vincolo sull'indirizzo
    non sul blocco
    io lo sto facendo anche sulla size del blocco.

    l'user-block dev'essere grande un multiplo di una w

    L'indirizzo che restituisci deve dare 0 se fai
    (uintptr_t)mem % sizeof(max_aligned_t)
 */

#include <stdlib.h>
int main(int argc, char *argv[]) {


    test_round_up_to_word();

    size_t   alloc_user_size = argc > 1 ? atol(argv[1]) : 113;
    size_t realloc_user_size = argc > 2 ? atol(argv[2]) : 12;


    // malign_block + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 9

    const posix_alignments alignment = AL_QWORD;
    for (uintptr_t p = 1; p < 1000; ++p) {

        const uint64_t aligned_size = round_up_to_word(1, alignment);
        const uint64_t malign_block = round_up_to_word(sizeof(void **), alignment);
        const uint64_t real_size    = malign_block + alignment + aligned_size;
        const uint64_t offset       = malign_block + (alignment - (p%alignment));

        printf("p=%zu real_size=%zu malign_blk=%zu aligned_size=%zu offset=%zu\n",
               p, real_size, malign_block, aligned_size, offset);

        uintptr_t user_address = p + offset;
        assert( user_address % alignment == 0); // address is aligned
        assert( user_address + aligned_size <= p + real_size);
        assert( user_address - malign_block >= p); // ensure i can read the metadata section

    }

    // 14 + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 9
    printf("%lu\n",
           32 + AL_QWORD + round_up_to_word(1, AL_QWORD)
    );
    //printf("%lu\n", round_up_to_word(alloc_user_size + 8, AL_WORD));
    return 0;

    void *p = malign_alloc(alloc_user_size, AL_EWORD);
    void *tmp = malign_realloc(p, realloc_user_size);
    if (tmp) p = tmp;

    malign_free(p);
    return 0;
}
