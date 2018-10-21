CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all:
	make objects
	make lds.a
	$(CC) -o main main.c $(CFLAGS) lds.a

objects:
	make -C binary_heap
	make -C doubly_linked_list
	make -C singly_linked_list
	make -C vector

lds.a: lds.a(binary_heap.o binary_heap_int.o doubly_linked_list.o fifo.o lifo.o vector.o vector_int.o vector_char.o)
	rm *.o

clean:
	rm -f *.o *.a main
