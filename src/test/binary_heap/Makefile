# CC=tcc
CFLAGS=-std=c89 -Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
.PHONY: all clean test

TST_TARGET_A=binary_heap_int
TST_TARGET_B=binary_heap_char

test: $(TST_TARGET_A).o $(TST_TARGET_B).o
	$(CC) $(CFLAGS) -o test test.c $(TST_TARGET_A).o $(TST_TARGET_B).o
	./test

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test