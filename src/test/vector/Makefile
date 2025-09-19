#CC=gcc
#CC=tcc
## CFLAGS=-std=c89 -Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
#CXXFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror -march=native -mtune=native -funswitch-loops -ftree-vectorize -fivopts -fmodulo-sched -flto

# since std::vector function are templates and im compiling .o object the inline of cpp is unfair my function cant be inlined if i link as .o
CXXFLAGS=-std=c++14 -fno-inline -Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror -march=native -mtune=native -funswitch-loops -ftree-vectorize -fivopts -fmodulo-sched

#CXXFLAGS=-O0 -g -fno-inline 
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
