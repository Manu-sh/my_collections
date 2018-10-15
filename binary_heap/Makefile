CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all: main.c binary_heap.h binary_heap.c
	$(CC)  $(CFLAGS) -c binary_heap.c
	$(CC)  -o main main.c binary_heap.o $(CFLAGS) 

clean:
	rm -f main
