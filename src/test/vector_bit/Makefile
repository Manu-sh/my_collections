CFLAGS=-O0 -pipe -Wall -Wextra -Wno-unused-function -pedantic -ftrapv -fstack-protector-all -fstack-protector-strong -fno-builtin-memcpy -fsanitize=address,undefined,null,alignment,leak,pointer-compare,pointer-subtract,unreachable -g3 -fno-omit-frame-pointer
.PHONY: all clean

all:
	$(CC) $(CFLAGS) -o test test.c

clean:
	rm -rf test
