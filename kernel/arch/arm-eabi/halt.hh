#ifndef HALT_H_INCLUDED
#define HALT_H_INCLUDED

static void inline halt() {
	__asm__ __volatile__ ("MCR p15, 0, r0, c7, c0, 4");
}

#endif
