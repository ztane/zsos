#ifndef __STACKTRACE_HH_INCLUDED__
#define __STACKTRACE_HH_INCLUDED__

#include <inttypes.h>
#include <stddef.h>


static __inline__ intptr_t getFramePointer() __attribute__((always_inline));

static __inline__ intptr_t getFramePointer()
{
    intptr_t rv;
    asm volatile("movl %%ebp, %0": "=r"(rv) : );
    return rv;
}

void getStackTrace(void *traceptrs[], size_t n_traceptrs, void *baseptr = 0);

#endif


