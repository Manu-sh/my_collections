#pragma once

#include "align_common.h"
#include "malign_meta.h"
#include <stdint.h>

// setup_block(user_ptr)


/*
    vincoli rispettati:
        - user_pointer punta ad un blocco che è grande un multiplo di alignment
        - user_pointer è un'indirizzo multiplo di alignment

        - user_pointer - sizeof(void *) è sempre accessibile ed è un puntatore,
           siccome ho bisogno di poterne modificare l'indirizzo devo però trattarlo come doppio puntatore

*/

// TODO: non fa il check dell'overflow sulle moltiplicazioni

// [pointer-metadata][user-memory]
void * malign_alloc(uint64_t size, posix_alignments alignment) {

    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer
    const uint8_t malign_blk_size = alignment;
    const uint64_t real_size = malign_blk_size + round_up_to_word(size, alignment); // reserved + aligned block
    uint8_t *real_block = (uint8_t *)malloc(real_size);

    if (UNLIKELY(!real_block))
        return NULL;

    #ifdef DEBUG
        printf("%s real_size=%lu\n", __func__, real_size);
    #endif

    // how many bytes i have to skip to have an address which is multiple of alignment
    // questo valore va salvato, non posso sapere quanti byte ho skippato
    const uint8_t offset = alignment - (
        ((uintptr_t)real_block) % alignment
    );

    void *user_pointer = (void *)(real_block + offset);

    // TODO: tecnicamente allineando a 8 non è possibile neanche usare SSE, quindi 8 come valore non ha molto senso
    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0); // ensure the pointer is properly aligned

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s offset=%hu\n", __func__, offset);
    #endif

    // ensure the block of memory is also aligned
    assert( (((uint8_t *)user_pointer) + round_up_to_word(size, alignment)) <= (real_block + real_size) );

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    malign_metadata *x = malign_meta_new(size, alignment, offset);
    *metadata = x;

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address-x: %p\n", __func__, (void *)x);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, (*metadata)->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, (*metadata)->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, malign_meta_user_real_size(*metadata));
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
    const malign_metadata *metadata = (const malign_metadata *)malign_meta_from_user_pointer(user_pointer);
    if (UNLIKELY(size == metadata->user_size))
        return user_pointer;

    // get the begging of the block
    void *real_block = ((uint8_t *)user_pointer) - metadata->offset;

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, malign_meta_user_real_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
    #endif


    // il vecchio blocco va liberato se cambia ma i metadati no
    // non posso usare realloc() perché può ritornare un indirizzo completamente diverso e a quel punto è inutile che abbia chiamato realloc()

    assert(metadata->user_alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer
    const uint8_t malign_blk_size = metadata->user_alignment;
    const uint64_t new_user_block_real_size = round_up_to_word(size, (posix_alignments)metadata->user_alignment);
    const uint64_t new_size = malign_blk_size + new_user_block_real_size; // reserved + aligned block

    #ifdef DEBUG
        printf("%s ask for blk size %lu\n", __func__, new_size);
    #endif

    uint8_t *new_block = (uint8_t *)malloc(new_size);
    if (UNLIKELY(!new_block))
        return NULL;

    // TODO: creare una funzione setup_block()
    uint8_t new_offset = metadata->user_alignment - (
        ((uintptr_t)new_block) % metadata->user_alignment
    );

    void *new_user_pointer = (void *)(new_block + new_offset);

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **new_metadata = (malign_metadata **) (
        ((uint8_t *)new_user_pointer) - sizeof(malign_metadata **)
    );

    // move the ownership of metadata pointer
    *new_metadata = (malign_metadata *)metadata; // remove the const qualifier, im the ownership now

    #ifdef DEBUG
        printf("%s src-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s src-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s src-block-real-size: %lu\n", __func__, malign_meta_user_real_size(metadata));

        printf("%s new-block-real-size: %lu\n", __func__, new_size);
    #endif

    // TODO: prendere min(a, b)

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, metadata->user_alignment),
        __builtin_assume_aligned(user_pointer, metadata->user_alignment),
        //metadata->user_size
        MIN( // TODO: migliorare sta roba: round_up_to_word(size, (posix_alignments)metadata->user_alignment)
            malign_meta_user_real_size(metadata),
            new_user_block_real_size
        )
    );

    free(real_block);

    // update offset and size metadata
    (*new_metadata)->offset    = new_offset;
    (*new_metadata)->user_size = size;

    #ifdef DEBUG
        printf("%s new-block-address: %p\n", __func__, new_block);
        printf("%s meta-address: %p\n", __func__, (void *)*new_metadata);
        printf("%s new-user-block-address: %p\n", __func__, new_user_pointer);
        printf("%s new-user-block-alignment: %hu\n", __func__, (*new_metadata)->user_alignment);
        printf("%s new-user-block-size: %lu\n", __func__, (*new_metadata)->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, malign_meta_user_real_size(*new_metadata));
        printf("%s new-offset: %hu\n", __func__, (*new_metadata)->offset);
        puts("");
    #endif

    return new_user_pointer;
}



void malign_free(void *user_pointer) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata *metadata = (malign_metadata *)malign_meta_from_user_pointer(user_pointer);

    // move the user pointer back to the begging of the block
    void *real_block = ((uint8_t *)user_pointer) - metadata->offset;

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        printf("%s user-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, malign_meta_user_real_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
    #endif

    malign_meta_free(metadata); // bye :(
    free(real_block);
}
