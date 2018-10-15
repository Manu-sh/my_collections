CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all: main.c doubly_linked_list.h doubly_linked_list.c
	$(CC) -c $(CFLAGS) doubly_linked_list.c
	$(CC)  -o main main.c $(CFLAGS) doubly_linked_list.o

clean:
	rm -f main
