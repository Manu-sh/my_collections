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
#include "../../allocators/posix/malign_posix.h"
#include "../../allocators/malign.h"

/*
la potenza di 2 è il vincolo sull'indirizzo
non sul blocco
io lo sto facendo anche sulla size del blocco.

l'user-block dev'essere grande un multiplo di una w


 L'indirizzo che restituisci deve dare 0 se fai
(uintptr_t)mem % sizeof(max_aligned_t)
 */
int main() {

    //void *p = malign_alloc(256000000, AL_EWORD);
    //p = p;

    printf("round_up=%lu\n", round_up_to_word(256000000, AL_EWORD));
    printf("round_up=%lu\n", round_up_to_word(113, AL_EWORD));


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
