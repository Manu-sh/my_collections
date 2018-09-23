CFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic
.PHONY: all clean test

all: main.c vector.h
	$(CC)  -o main main.c $(CFLAGS) 

clean:
	rm -f main
