#CC=gcc
#CC=tcc
# CFLAGS=-std=c89 -Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
CXXFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
.PHONY: all clean test

TST_TARGET_A=vector_char
TST_TARGET_B=vector_int
TST_TARGET_C=vector_void_ptr

all: $(TST_TARGET_A).o $(TST_TARGET_B).o $(TST_TARGET_C).o
	# $(CC) $(CFLAGS) -o test test.c $(TST_TARGET_A).o $(TST_TARGET_B).o $(TST_TARGET_C).o
	$(CXX) $(CXXFLAGS) -o test test.cpp $(TST_TARGET_A).o $(TST_TARGET_B).o
	./test

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

single_blob:
	$(CC) $(CFLAGS) -o single_blob single_blob.c

test: all
	./test

clean:
	rm -f *.o test single_blob
