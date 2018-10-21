CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all: binary_heap.o binary_heap_int.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o ../$@

clean:
	rm -f *.o
