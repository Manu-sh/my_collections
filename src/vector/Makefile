CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all: vector.o vector_int.o vector_char.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o ../$@

clean:
	rm -f *.o