#include "../../vector_bit/vector_bit.h"
#include <stdio.h>
#include <immintrin.h>

int main() {

    vector_bit *vct = vector_bit_new();

	if ( !vector_bit_resize(vct, 8 * 256000000u) ) {
        perror("oops");
        return EXIT_FAILURE;
    }

	// perf stat -e task-clock,cycles,instructions,r1b1,r10e,stalled-cycles-frontend,stalled-cycles-backend,L1-dcache-load-misses,cache-misses ./benchmark
    for (uint_fast64_t i = 0; i < 8 * 256000000u; ++i) { // 256MB of bits
        //vector_bit_push(vct, !(i & 1));
        vector_bit_push(vct, 1);
        //printf("bit[%d] = %d\n", i, vector_bit_access(vct, i));
    }


    // int times = 0;
    /*
    while (!vector_bit_is_empty(vct)) {
        //printf("step %d -> %d\n", ++times, vector_bit_pop(vct));
        vector_bit_pop(vct);
        //vector_bit_fast_pop(vct);
    }*/

    //assert(vector_bit_length(vct) == 0);
    //printf("capacity %zu\n", vector_bit_capacity(vct));
    assert(vector_bit_capacity(vct) >= 8);

    vector_bit_free(vct);
    return 0;
}
