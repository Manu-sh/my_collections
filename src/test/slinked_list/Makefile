# CC=clang
CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

TST_TARGET_A=fifo_int
TST_TARGET_B=lifo_int

all: $(TST_TARGET_A).o $(TST_TARGET_B).o
	$(CC) $(CFLAGS) -o test test.c $(TST_TARGET_A).o $(TST_TARGET_B).o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	./test

clean:
	rm -f *.o test
