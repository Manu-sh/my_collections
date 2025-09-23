#pragma once

#include "align_common.h"
#include "malign_meta.h"
#include <stdint.h>

/*
 * TODO:
    la potenza di 2 è il vincolo sull'indirizzo
    non sul blocco
    io lo sto facendo anche sulla size del blocco
 */
// TODO: non fa il check dell'overflow sulle moltiplicazioni

// [pointer-metadata][user-memory]
void * malign_alloc(uint64_t size, posix_alignments alignment) {

    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer
    const uint64_t malign_blk_size = alignment;

    uint64_t real_size = malign_blk_size + round_up_to_word(size, alignment); // reserved + aligned block
    uint8_t *real_block = (uint8_t *)malloc(real_size);

    if (UNLIKELY(!real_block))
        return NULL;

    #ifdef DEBUG
        printf("%s real_size=%lu\n", __func__, real_size);
    #endif

    // how many bytes i have to skip to have an address which is multiple of alignment
    // questo valore va salvato, non posso sapere quanti byte ho skippato
    int64_t offset = alignment - (
        ((uintptr_t)real_block) % alignment
    );
    assert(offset > 0 && offset < 256);

    void *user_pointer = (void *)(real_block + offset);

    // TODO: tecnicamente allineando a 8 non è possibile neanche usare SSE, quindi 8 come valore non ha molto senso
    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0); // ensure the pointer is properly aligned

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s offset=%lu\n", __func__, offset);
    #endif

    // ensure the block of memory is also aligned
    assert( (((uint8_t *)user_pointer) + round_up_to_word(size, alignment)) <= (real_block + real_size) );


    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    malign_metadata *x = malign_meta_new(size, alignment, offset);
    *metadata = x; //*metadata = (malign_metadata *)(uintptr_t)0xdeadbeef;

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address-x: %p\n", __func__, (void *)x);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, (*metadata)->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, (*metadata)->user_size);
        printf("%s offset: %hu\n", __func__, (*metadata)->offset);
        puts("");
    #endif

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );

}


void * malign_realloc(void *user_pointer, uint64_t size) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    const malign_metadata **metadata = (const malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    if (UNLIKELY(size == (*metadata)->user_size))
        return user_pointer;


    // TODO: move the user pointer back to the begging of the block
    void *real_block = ((uint8_t *)user_pointer) - (*metadata)->offset;

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, (*metadata)->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, (*metadata)->user_size);
        puts("");
    #endif


    // il vecchio blocco va liberato se cambia ma i metadati no

    // TODO: non posso usare realloc() perché può ritornare un indirizzo completamente diverso e a quel punto è inutile che abbia chiamato realloc()


    uint8_t alignment = (*metadata)->user_alignment;
    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer
    const uint8_t malign_blk_size = alignment;

    uint64_t new_size = malign_blk_size + round_up_to_word(size, (posix_alignments)alignment); // reserved + aligned block

    #ifdef DEBUG
        printf("%s ask for blk size %lu\n", __func__, new_size);
    #endif

    uint8_t *new_block = (uint8_t *)malloc(new_size);

    if (UNLIKELY(!new_block))
        return NULL;

    int64_t new_offset = alignment - (
        ((uintptr_t)new_block) % alignment
    );
    assert(new_offset > 0 && new_offset < 256);

    void *new_user_pointer = (void *)(new_block + new_offset);

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **new_metadata = (malign_metadata **) (
        ((uint8_t *)new_user_pointer) - sizeof(malign_metadata **)
    );

    // move the ownership of metadata pointer
    *new_metadata = (malign_metadata *)*metadata; // remove the const qualifier, im the ownership now

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, (*metadata)->user_alignment),
        __builtin_assume_aligned(user_pointer, (*metadata)->user_alignment),
        (*metadata)->user_size
    );

    free(real_block);

    // update offset and size metadata
    (*new_metadata)->offset    = new_offset;
    (*new_metadata)->user_size = size;

    return new_user_pointer;
}