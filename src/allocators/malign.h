#pragma once
#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
    #define _POSIX_C_SOURCE 200112L
#endif

#include "align_common.h"
#include "malign_meta.h"
#include <stdint.h>

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #include <stdio.h>
#endif

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
        printf("%s ask for blk size %lu\n", __func__, real_size);
    #endif


    void *tmp_block = NULL;
    const int ec = posix_memalign(&tmp_block, alignment, real_size);

    if (UNLIKELY(ec))
        return NULL;

    // setup metadata
    void *const real_block = tmp_block;
    void *const user_pointer = ((uint8_t *)real_block) + malign_blk_size;

    #ifdef DEBUG
        puts("still alive");
    #endif


    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
            ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    *metadata = malign_meta_new(size, alignment);
    //*metadata = (malign_metadata *)(uintptr_t)0xdeadbeef;

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

void * malign_realloc(void *user_pointer, size_t size) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    if (UNLIKELY(size == (*metadata)->user_size))
        return user_pointer;

    // move the user pointer back to the begging of the block
    void *real_block = ((uint8_t *)user_pointer) - (*metadata)->user_alignment;

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        puts("");
    #endif

    // il vecchio blocco va liberato ma i metadati no
    // TODO: bisogna per forza allineare un blocco di alignment size in più, di questo blocco bisogna però leggere SOLO gli ultimi 8 byte

    assert(sizeof(malign_metadata **) <= (*metadata)->user_alignment);
    const uint64_t malign_blk_size = (*metadata)->user_alignment;

    uint64_t real_size = malign_blk_size + size;   // reserve size for metadata
    real_size = calc_align_size_based(real_size, (*metadata)->user_alignment); // -1 because real_size is not an index and doesn't require to be accessible

    #ifdef DEBUG
        printf("%s ask for blk size %lu\n", __func__, real_size);
    #endif

    void *tmp_block = NULL;
    const int ec = posix_memalign(&tmp_block, (*metadata)->user_alignment, real_size);
    if (UNLIKELY(ec))
        return NULL; // leave the block untouched

    // setup metadata
    void *const new_block = tmp_block;
    void *const new_user_pointer = ((uint8_t *)new_block) + malign_blk_size;

    #ifdef DEBUG
        puts("still alive");
    #endif

    // TODO: può darsi che basti usare memcpy a diritto
    // setup metadata

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **new_metadata = (malign_metadata **) (
        ((uint8_t *)new_user_pointer) - sizeof(malign_metadata **)
    );

    // move the metadata pointer
    *new_metadata = *metadata;
    (*new_metadata)->user_size = size;


    #ifdef DEBUG
        puts("ready for memcpy");
    #endif


    assert( ((uintptr_t)new_user_pointer) % (*metadata)->user_alignment == 0);
    assert( ((uintptr_t)user_pointer)     % (*metadata)->user_alignment == 0);

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, (*metadata)->user_alignment),
        __builtin_assume_aligned(user_pointer, (*metadata)->user_alignment),
        real_size
    );

    #ifdef DEBUG
        printf("%s new-block-address: %p\n", __func__, new_block);
        printf("%s new-meta-address: %p\n", __func__, (void *)*new_metadata);
        printf("%s new-user-address: %p\n", __func__, new_user_pointer);
        puts("");
    #endif

    free(real_block);
    return new_user_pointer;
}


void malign_free(void *user_pointer) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    // move the user pointer back to the begging of the block
    void *real_block = ((uint8_t *)user_pointer) - (*metadata)->user_alignment;

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        puts("");
    #endif

    malign_meta_free(*metadata);
    free(real_block);
}