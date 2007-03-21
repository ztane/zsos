#ifndef HALT_H_INCLUDED
#define HALT_H_INCLUDED

static void inline halt() {
	__asm__ __volatile__ ("hlt");
}

#endif
