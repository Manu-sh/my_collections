#pragma once

#ifdef FORCED
    #ifndef __cplusplus // #warning is cpp23 feature :/
        #warning "FORCED() macro already defined, inline may not performed"
    #endif
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


#define LIKELY(_EXP_)       __builtin_expect(_EXP_, 1)
#define UNLIKELY(_EXP_)     __builtin_expect(_EXP_, 0)