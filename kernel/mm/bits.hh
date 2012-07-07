#ifndef __MM_TYPES_INCLUDED__
#define __MM_TYPES_INCLUDED__

#include <inttypes.h>

static inline uintptr_t frameNumberFromLinear(void *a) {
	return (((intptr_t)a - 0xC0000000UL) >> 12);
}

static inline void *linearFromFrameNumber(uintptr_t addr) {
	return (void*)(0xC0000000UL + (addr << 12));
}

#endif
