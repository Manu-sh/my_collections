#pragma once

#include "align_common.h"
#include "malign_meta.h"
#include <stdint.h>

#define __helper static FORCED(inline)

// setup_block(user_ptr)

/*
    vincoli rispettati:
        - user_pointer punta ad un blocco che è grande un multiplo di alignment
        - user_pointer è un'indirizzo multiplo di alignment

        - user_pointer - sizeof(void *) è sempre accessibile ed è un puntatore,
           siccome ho bisogno di poterne modificare l'indirizzo devo però trattarlo come doppio puntatore


        alignment = 8 byte

        24 byte block:
            malign_block = round_up_to_word(sizeof(void **), alignment) // 8
            malloc( malign_block + alignment + round_up_to_word(size, alignment) ) // + alignment is because the pointer could be misaligned and we need to adjust his address

        real_blk = (0x07, ..., 0x1e) -> (0x07, 0x1f]

        new_offset   = malign_block + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 9
        user_pointer = real_blk + new_offset -> 0x10
        real_blk     = user_pointer - new_offset -> (0x10-9) = 0x07

        0x1e - 0x10 -> 15 byte (0x1e - 0x10 = 14 ma siccome 0x1e non è end ma un'indirizzo accessibile sono 15 byte)

        aligned_size riporta la size del blocco ma se fosse 14 memcpy non farebbe ottimizzazioni,
        per questo l'helper riporta la size richiesta dall'utente ma allineata.


        // TODO: test con 0x01

        alignment = 32 byte

        96 byte block:
            malign_block = round_up_to_word(sizeof(void **), alignment) // 32
            malloc( malign_block + alignment + round_up_to_word(size, alignment) ) // + alignment is because the pointer could be misaligned and we need to adjust his address

        real_blk = (0x07, ..., 0x66) -> (0x07, 0x67]

        new_offset   = malign_block + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 57
        user_pointer = real_blk + new_offset -> 0x40
        real_blk     = user_pointer - new_offset -> (0x40-57) = 0x07

        0x67-0x40 -> 39 byte

    c'è il fatto che malloc() su linux ritorna indirizzi multipli di 8 quindi per testare bisognerebbe fare un mock() di malloc
    sfruttando .data oppure allocando un blocco grosso con malloc e restituendo indirizzi disallineati

*/



// TODO: non fa il check dell'overflow sulle moltiplicazioni

__helper malign_metadata ** __get_meta(void *user_pointer) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    assert(metadata);
    return metadata;
}

__helper malign_metadata * get_meta(void *user_pointer) {
    return *__get_meta(user_pointer);
}

__helper malign_metadata * set_meta(void *user_pointer, malign_metadata *metadata) {
    return *__get_meta(user_pointer) = metadata, metadata;
}

// recover the effective size of block pointed by user_pointer
// TODO: probabilmente conviene passare questo valore a memcpy() visto che dovrebbe favorire la vettorizzazione
__helper uint64_t user_block_aligned_size(const malign_metadata *metadata) {
    return malign_meta_aligned_size(metadata);
}

__helper void * get_real_block(void *user_pointer, const malign_metadata *metadata) {
    return ((uint8_t *)user_pointer) - metadata->offset;
}

__helper uint64_t get_real_block_size(void *user_pointer, const malign_metadata *metadata) {    // real_block: [opt_pad + metadata + user_block]
    const uint8_t *const end = (((uint8_t *)user_pointer) + user_block_aligned_size(metadata)); // move at the end of user_block which is also the end of real_block
    const uint8_t *const beg = ((uint8_t *)get_real_block(user_pointer, metadata));             // get the beginning of the real_block
    return ((uintptr_t)end) - ((uintptr_t)beg);
}

// calcola l'offset dell'user_pointer rispetto a real_block perché l'indirizzo abbia un dato allineamento metadata->alignment
// void *user_pointer = real_block + offset;
__helper uint8_t calc_offset(void *real_block, posix_alignment alignment) {
    // alignment will be a pow of 2: a%b -> a&(b-1)
    return round_up_to_word(sizeof(void **), alignment) + (alignment - (
        ((uintptr_t)real_block) & (alignment - 1) // return alignment - ( ((uintptr_t)real_block) % alignment );
    ));
}

__helper void * get_user_block(void *real_block, posix_alignment alignment) {
    return ((uint8_t *)real_block) + calc_offset(real_block, alignment);
}

__helper uint64_t get_user_block_aligned_size(void *user_pointer) {
    return user_block_aligned_size(get_meta(user_pointer));
}

// [opt-padding + pointer-metadata][user-memory][opt-padding]
void * malign_alloc(uint64_t size, posix_alignment alignment) {

    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer

    // padding + reserved + aligned block: padding is required to move the user_pointer at the right address to be aligned
    const uint64_t malign_block = round_up_to_word(sizeof(void **), alignment);
    const uint64_t aligned_size = round_up_to_word(size, alignment);
    const uint64_t real_size    = malign_block + alignment + aligned_size;

    uint8_t *real_block = (uint8_t *)malloc(real_size);
    if (UNLIKELY(!real_block))
        return NULL;

    #ifdef DEBUG
        printf("%s real_size=%lu\n", __func__, real_size);
    #endif

    // how many bytes i have to skip to have an address which is multiple of alignment
    // questo valore va salvato, non posso sapere quanti byte ho skippato
    void *user_pointer = get_user_block(real_block, alignment);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s offset=%hu\n", __func__, calc_offset(real_block, alignment));
    #endif

    // tecnicamente allineando a 8 non è possibile neanche usare SSE, quindi 8 come valore non ha molto senso
    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0); // ensure the pointer is properly aligned
    assert( (((uintptr_t)((uint8_t *)user_pointer)) & (alignment-1)) == 0); // ensure the pointer is properly aligned

    // ensure the block of memory is also aligned
    assert( (((uint8_t *)user_pointer) + aligned_size) <= (real_block + real_size) );

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata *x = malign_meta_new(size, alignment, calc_offset(real_block, alignment));
    if (UNLIKELY(!x)) {
        free(real_block);
        return NULL;
    }

    set_meta(user_pointer, x);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address-x: %p\n", __func__, (void *)x);
        printf("%s meta-address: %p\n", __func__, (void *)get_meta(user_pointer));
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, get_meta(user_pointer)->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, get_meta(user_pointer)->user_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, user_block_aligned_size(get_meta(user_pointer)));
        printf("%s offset: %hu\n", __func__, get_meta(user_pointer)->offset);
        puts("");

        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(get_meta(user_pointer)));
    #endif

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );

}


void * malign_realloc(void *user_pointer, uint64_t size) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    const malign_metadata *metadata = get_meta(user_pointer);
    if (UNLIKELY(size == metadata->user_size))
        return user_pointer;

    // se l'utente richiede più memoria ma questa è comunque meno della size allineata allora non fare nulla
    // anche se size < get_user_block_aligned_size() e size > (metadata->user_size) non bisogna fare niente eccetto aggiornare user_size
    // es. ho chiesto 60 byte l'userblock ha size allineata a 64, l'utente richiede di allargarlo a 63 la size allineata è 64 quindi non faccio niente
    if (size > metadata->user_size && size < get_user_block_aligned_size(user_pointer)) {
        ((malign_metadata *)metadata)->user_size = size; // edit the user_size
        return user_pointer;
    }

    // get the begging of the block
    void *real_block = get_real_block(user_pointer, metadata);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s user-block-real-size: %lu\n", __func__, get_real_block_size(user_pointer, metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
    #endif

    // il vecchio blocco va liberato se cambia ma i metadati no
    // non posso usare realloc() perché può ritornare un indirizzo completamente diverso e a quel punto è inutile che abbia chiamato realloc()

    assert(metadata->user_alignment >= sizeof(void **)); // enough space for metadata pointer

    // padding + reserved + aligned block: padding is required to move the user_pointer at the right address to be aligned
    const posix_alignment alignment = (posix_alignment)metadata->user_alignment;
    const uint64_t malign_block     = round_up_to_word(sizeof(void **), alignment);
    const uint64_t new_aligned_size = round_up_to_word(size, alignment);
    const uint64_t new_real_size    = malign_block + alignment + new_aligned_size;

    #ifdef DEBUG
        printf("%s ask for blk size %lu\n", __func__, new_real_size);
    #endif

    uint8_t *new_real_block = (uint8_t *)malloc(new_real_size);
    if (UNLIKELY(!new_real_block))
        return NULL;

    void *new_user_pointer = get_user_block(new_real_block, alignment);

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    set_meta(new_user_pointer, (malign_metadata *)metadata); // assign metadata the pointer address and remove the const qualifier, im the ownership now

    #ifdef DEBUG
        printf("%s src-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s src-block-size: %lu\n", __func__, metadata->user_size);
        printf("%s src-block-aligned-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s new-block-real-size: %lu\n", __func__, new_real_size);
        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
    #endif

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, metadata->user_alignment),
        __builtin_assume_aligned(user_pointer, metadata->user_alignment),
        MIN( // the new block could be smaller
            round_up_to_word(metadata->user_size, (posix_alignment)metadata->user_alignment),
            new_aligned_size
        )
    );

    free(real_block);

    // update offset and size metadata
    malign_metadata *update_metadata = get_meta(new_user_pointer);
    update_metadata->offset          = calc_offset(new_real_block, alignment);
    update_metadata->user_size       = size;

    #ifdef DEBUG
        printf("%s new-block-address: %p\n", __func__, new_real_block);
        printf("%s meta-address: %p\n", __func__, (void *)update_metadata);
        printf("%s new-user-block-address: %p\n", __func__, new_user_pointer);
        printf("%s new-user-block-alignment: %hu\n", __func__, update_metadata->user_alignment);
        printf("%s new-user-block-size: %lu\n", __func__, update_metadata->user_size);
        printf("%s new-user-block-aligned-size: %lu\n", __func__, user_block_aligned_size(update_metadata));
        printf("%s new-offset: %hu\n", __func__, (update_metadata)->offset);
        puts("");
        assert(get_user_block_aligned_size(new_user_pointer) == user_block_aligned_size(update_metadata));
        //assert(0);
    #endif

    return new_user_pointer;
}


void malign_free(void *user_pointer) {

    // gli ultimi 8 byte di real_block sono un puntatore ai metadati
    malign_metadata *metadata = get_meta(user_pointer);

    // move the user pointer back to the begging of the block
    //void *real_block = ((uint8_t *)user_pointer) - metadata->offset;
    void *real_block = get_real_block(user_pointer, metadata);

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        printf("%s user-alignment: %hu\n", __func__, metadata->user_alignment);
        printf("%s user-size: %lu\n", __func__, metadata->user_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, user_block_aligned_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
        assert(get_user_block_aligned_size(user_pointer) == user_block_aligned_size(metadata));
    #endif

    malign_meta_free(metadata); // bye :(
    free(real_block);
}
