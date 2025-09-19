CFLAGS=-std=c99 -O0 -pipe -Wall -Wextra -Wno-unused-function -pedantic -ftrapv -fstack-protector-all -fstack-protector-strong -fno-builtin-memcpy -fsanitize=address,undefined,null,alignment,leak,pointer-compare,pointer-subtract,unreachable -g3 -fno-omit-frame-pointer
.PHONY: all clean

all:
	$(CC) $(CFLAGS) -o test test.c

build_benchmark:
	$(CC) -std=c99 -Ofast -ffast-math -funroll-loops -march=native -mtune=native -funswitch-loops -ftree-vectorize -fivopts -fmodulo-sched -flto -pipe -Wall -Wextra -Wno-unused-function -pedantic -fno-builtin-memcpy benchmark.c -o benchmark

benchmark: build_benchmark
	time ./benchmark

test: all
	./test

clean:
	rm -rf test benchmark
