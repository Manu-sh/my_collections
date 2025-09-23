#pragma once
#include "allocator_posix_align.h"
#include "malign_meta.h"
#include <stdint.h>

// malign: memory aligned

// TODO: per garantire l'allineamento devo allineare il blocco dei metadati stesso però a quel punto diviene un problema risolvere se si usa una dimensione variabile
//  siccome l'allineamento richiesto da posix_malloc è che sia allineato a un multiplo della word posso ficcare un ptr davanti all'user pointer
//  bisogna per forza allineare un blocco di alignment size in più, di questo blocco bisogna però leggere SOLO gli ultimi 8 byte

// [pointer-metadata][user-memory]

void * malign_alloc(size_t size, posix_alignments alignment) {

    // TODO: bisogna per forza allineare un blocco di alignment size in più, di questo blocco bisogna però leggere SOLO gli ultimi 8 byte

    assert(sizeof(malign_metadata **) <= alignment);

    const uint64_t malign_blk_size = alignment;

    uint64_t real_size = malign_blk_size + size;   // reserve size for metadata
    real_size = calc_align_size_based(real_size, alignment); // -1 because real_size is not an index and doesn't require to be accessible

    #ifdef DEBUG
        printf("ask for blk size %lu\n", real_size);
    #endif


    void *tmp_block = NULL;
    const int ec = posix_memalign(&tmp_block, alignment, real_size);

    if (UNLIKELY(ec))
        return NULL;

    // setup metadata
    void *const real_block = tmp_block;

    void *const user_pointer = ((uint8_t *)real_block) + malign_blk_size;

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
            ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );


    //*metadata = malign_meta_new(size, alignment);
    *metadata = (malign_metadata *)(uintptr_t)0xdeadbeef;

#ifdef DEBUG
    printf("%s block-address: %p\n", __func__, real_block);
    printf("%s meta-address: %p\n", __func__, (void *)*metadata);
    printf("%s user-address: %p\n", __func__, user_pointer);
    puts("");
#endif

    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0);

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );
}

#if 0
// TODO: here
void * malign_realloc(void *p, size_t size) {

    malloc_metadata *real_block = (malloc_metadata *) (((uint8_t *)p) - (
        sizeof(malloc_metadata)
    ));

#ifdef DEBUG
    printf("user-address: %p\n", p);
    printf("calc address: %p\n", (void *)real_block);
#endif

    if (UNLIKELY(size == real_block->user_size))
        return p;

    void *user_memory = malign_alloc(size, (posix_alignments)real_block->user_alignment);
    if (UNLIKELY(!user_memory))
        return NULL; // leave the block untouched


    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(user_memory, real_block->user_alignment),
        __builtin_assume_aligned(p, real_block->user_alignment),
        real_block->user_size
    );

    free(real_block);
    return user_memory;
}
#endif


void malign_free(void *user_pointer) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    // move the user pointer back to the begging of the block
    //void *real_block = ((uint8_t *)user_pointer) - (*metadata)->user_alignment;

#ifdef DEBUG
    //printf("%s block-address: %p\n", __func__, real_block);
    printf("%s meta-address: %p\n", __func__, (void *)*metadata);
    printf("%s user-address: %p\n", __func__, user_pointer);
    puts("");
#endif

    //malign_meta_free(*metadata);
    //free(real_block);
}