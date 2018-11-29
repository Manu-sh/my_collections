# CC=tcc
#CFLAGS=-std=c89 -Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
CXXFLAGS=-Ofast -pipe -ffast-math -Wall -Wextra -D_REENTRANT -Wno-unused-function -fstack-protector-all -fstack-protector-strong -funroll-loops -pedantic -Werror
.PHONY: all clean test

TST_TARGET_A=priority_queue_int
TST_TARGET_B=priority_queue_char

test: $(TST_TARGET_A).o $(TST_TARGET_B).o
	# $(CC) $(CFLAGS) -o test test.c $(TST_TARGET_A).o $(TST_TARGET_B).o
	$(CXX) $(CXXFLAGS) -o test test.cpp $(TST_TARGET_A).o $(TST_TARGET_B).o
	./test

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test
