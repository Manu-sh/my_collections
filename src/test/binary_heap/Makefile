CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

TST_TARGET=binary_heap_int

test: $(TST_TARGET).o
	$(CC) $(CFLAGS) -o test test.c $(TST_TARGET).o
	./test

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test