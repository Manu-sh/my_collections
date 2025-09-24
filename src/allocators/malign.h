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

static FORCED(inline) malign_metadata ** get_meta(void *user_pointer) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    assert(metadata);
    return metadata;
}

// recover the effective size of block pointed by user_pointer
// TODO: probabilmente conviene passare questo valore a memcpy() visto che dovrebbe favorire la vettorizzazione
static FORCED(inline) uint64_t user_block_aligned_size(const malign_metadata *metadata) {
    return malign_meta_aligned_size(metadata);
}

static FORCED(inline) void * get_real_block(const malign_metadata *metadata, void *user_pointer) {
    return ((uint8_t *)user_pointer) - metadata->offset;
}

static FORCED(inline) uint64_t get_real_block_size(const malign_metadata *metadata, void *user_pointer) {
    const uint8_t *const end = (((uint8_t *)user_pointer) + user_block_aligned_size(metadata));
    const uint8_t *const beg = ((uint8_t *)get_real_block(metadata, user_pointer));
    return ((uintptr_t)end) - ((uintptr_t)beg);
}

// calcola l'offset dell'user_pointer rispetto a real_block perché l'indirizzo abbia un dato allineamento metadata->alignment
// void *user_pointer = real_block + offset;
static FORCED(inline) uint8_t calc_offset(void *real_block, uint8_t alignment) {
    // TODO: provare a ottimizzare % sarà una potenza di 2: a%b -> a&(b-1)
    /*
    return alignment - (
        ((uintptr_t)real_block) % alignment
    );
     */

    return alignment - (
        ((uintptr_t)real_block) & (alignment - 1)
    );
}

static FORCED(inline) void * get_user_block(void *real_block, uint8_t alignment) {
    return ((uint8_t *)real_block) + calc_offset(real_block, alignment);
}


static FORCED(inline) uint64_t get_user_block_aligned_size(void *user_pointer) {
    return user_block_aligned_size(*get_meta(user_pointer));
}

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
    void *user_pointer = get_user_block(real_block, alignment);

    // TODO: tecnicamente allineando a 8 non è possibile neanche usare SSE, quindi 8 come valore non ha molto senso
    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0); // ensure the pointer is properly aligned
    assert( (((uintptr_t)((uint8_t *)user_pointer)) & (alignment-1)) == 0); // ensure the pointer is properly aligned

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s offset=%hu\n", __func__, calc_offset(real_block, alignment));
    #endif

    // ensure the block of memory is also aligned
    assert( (((uint8_t *)user_pointer) + round_up_to_word(size, alignment)) <= (real_block + real_size) );

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata *x = malign_meta_new(size, alignment, calc_offset(real_block, alignment));
    if (UNLIKELY(!x)) {
        free(real_block);
        return NULL;
    }

    malign_metadata **metadata = get_meta(user_pointer);
    *metadata = x;

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address-x: %p\n", __func__, (void *)x);
        printf("%s meta-address: %p\n", __func__, (void *)*metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, (*metadata)->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, (*metadata)->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, user_block_aligned_size(*metadata));
        printf("%s offset: %hu\n", __func__, (*metadata)->offset);
        puts("");

        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(*metadata));
    #endif

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );

}


void * malign_realloc(void *user_pointer, uint64_t size) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    const malign_metadata *metadata = *get_meta(user_pointer);
    if (UNLIKELY(size == metadata->user_size))
        return user_pointer;

    // get the begging of the block
    void *real_block = get_real_block(metadata, user_pointer);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");

        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
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

    uint8_t new_offset = calc_offset(new_block, metadata->user_alignment);
    void *new_user_pointer = (void *)(new_block + new_offset);

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **new_metadata = get_meta(new_user_pointer);
    *new_metadata = (malign_metadata *)metadata; // assign metadata the pointer address and remove the const qualifier, im the ownership now

    #ifdef DEBUG
        printf("%s src-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s src-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s src-block-real-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s new-block-real-size: %lu\n", __func__, new_size);
        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
    #endif

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, metadata->user_alignment),
        __builtin_assume_aligned(user_pointer, metadata->user_alignment),
        MIN( // metadata->user_size
            user_block_aligned_size(metadata),
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
        printf("%s user-block-real-size: %lu\n", __func__, user_block_aligned_size(*new_metadata));
        printf("%s new-offset: %hu\n", __func__, (*new_metadata)->offset);
        puts("");
        assert(get_user_block_aligned_size(new_user_pointer) == user_block_aligned_size(*new_metadata));
        //assert(0);
    #endif

    return new_user_pointer;
}


void malign_free(void *user_pointer) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata *metadata = *get_meta(user_pointer);

    // move the user pointer back to the begging of the block
    //void *real_block = ((uint8_t *)user_pointer) - metadata->offset;
    void *real_block = get_real_block(metadata, user_pointer);

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        printf("%s user-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-real-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
    #endif

    malign_meta_free(metadata); // bye :(
    free(real_block);
}
