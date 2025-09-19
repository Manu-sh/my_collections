/* only g*.c should include this header */
#pragma once
#ifndef CCOMMON_H
	#include "ccommon.h"
#endif


#ifdef FORCED
    #warning "FORCED() macro already defined, inline may not performed"
#endif


/*
 NOTE: to use inline you need to compile at least with c99, you can disable inline defining FORCED(_) as a macro that do nothing
 __STDC_VERSION__ is a macro defined with c95 (199409L)
*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    #define FORCED(_)
#endif


/* just hope that is a real inline */
#ifndef FORCED
    #define FORCED(_UNUSED_) inline __attribute__((always_inline))
#endif


#define __helper static FORCED(inline)

#define SWAP(_TYPENAME_,A,B)          \
	do {                      \
		_TYPENAME_ _tmp_ = A; \
		A = B;            \
		B = _tmp_;        \
	} while(0)
