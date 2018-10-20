#pragma once
#define __always_inline __attribute__((always_inline)) inline
#define __helper static __always_inline

#define TOKENPASTE(_A_, _B_, _C_) _A_ ## _B_ ## _C_
#define TKPASTE(_A_, _B_, _C_) TOKENPASTE(_A_, _B_, _C_)
#define _(_NAME_) TKPASTE(FNPREFIX, FNTYPE, _NAME_)

#define SWAP(_TYPE_,A,B)          \
	do {                      \
		_TYPE_ _tmp_ = A; \
		A = B;            \
		B = _tmp_;        \
	} while(0)