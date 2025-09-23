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
#include "../../allocators/posix/allocator_posix_align.h"
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
int main() {

    test_round_up_to_word();

    void *p = malign_alloc(113, AL_EWORD);
    void *tmp = malign_realloc(p, 12);
    if (tmp) p = tmp;

    malign_free(p);
    return 0;

    #if 0
        //void *p = malign_alloc(113, AL_WORD);
        void *p = malign_posix_alloc(113, AL_WORD);
        //void *p = malign_alloc(113, AL_EWORD);

        void *tmp = malign_posix_realloc(p, 256000000);
        if (tmp) p = tmp;

        malign_posix_free(p);
        return 0;

        allocator_posix_align *al = allocator_posix_align_new(120, AL_WORD);

        printf("alignment: %u\n", al->alignment);
        printf("size: %lu\n", al->size);
        printf("p: %p\n", al->p);

        allocator_posix_align_grow(al, 1020);
        printf("alignment: %u\n", al->alignment);
        printf("size: %lu\n", al->size);
        printf("p: %p\n", al->p);


        allocator_posix_align_shrink(al, 712);
        printf("alignment: %u\n", al->alignment);
        printf("size: %lu\n", al->size);
        printf("p: %p\n", al->p);

        allocator_posix_align_free(al);
        return 0;
    #endif
}
