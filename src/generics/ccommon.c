/* only g*.c should include this header */

#pragma once
#ifndef CCOMMON_H
	#include "ccommon.h"
#endif

#define __always_inline __attribute__((always_inline)) inline
#define __helper static __always_inline

#define SWAP(_TYPENAME_,A,B)          \
	do {                      \
		_TYPENAME_ _tmp_ = A; \
		A = B;            \
		B = _tmp_;        \
	} while(0)
