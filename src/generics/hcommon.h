/* CLASSNAME & TYPENAME MUST BE DEFINED BEFORE INCLUDING THIS HDR
	example:
		#define CLASSNAME vector
		#define TYPENAME char
*/

// #pragma once
#include <stdbool.h>

#if !defined(CLASSNAME) || !defined(TYPENAME)
	#error "you must define CLASSNAME & TYPENAME"
#endif

#define UNDERSCORE _
#define EMPTY_DEF

#define TOKENPASTE(_A_, _B_, _C_) _A_ ## _B_ ## _C_
#define TKPASTE(_A_, _B_, _C_) TOKENPASTE(_A_, _B_, _C_)

#define _(_NAME_) TKPASTE(CLASSNAME, TKPASTE(UNDERSCORE, TYPENAME, UNDERSCORE), _NAME_) /* _(popBack) -> vector_char_popBack */
#define STRUCT    TKPASTE(CLASSNAME, UNDERSCORE, TYPENAME) /* example: vector_char */
typedef struct STRUCT STRUCT;
