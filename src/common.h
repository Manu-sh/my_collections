#pragma once
#define __helper static __attribute__((always_inline)) inline

#define SWAP(_TYPE_,A,B)          \
	do {                      \
		_TYPE_ _tmp_ = A; \
		A = B;            \
		B = _tmp_;        \
	} while(0)
