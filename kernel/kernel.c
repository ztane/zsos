#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mm/kmalloc.h"
#include "kernelmessages.hh"
#include "printk.h"

extern void (*__CTOR_LIST__)();

static void call_ctors()
{
	void (**fptr)() = (&__CTOR_LIST__) + 1;
	int count = ((int *)(&__CTOR_LIST__))[0];
	printk("Executing %d static C++ initializers...", count);

	while (count--) {
		(*fptr++)();
        }

	printk(" done\n");
}


#ifdef __arm__

extern void kernel_main(uint32_t, uint32_t, uint32_t);
void kmain(uint32_t r0, uint32_t r1, uint32_t r2) {

#else
extern void kernel_main(unsigned long, void *);
void kmain(uint32_t magic, void *addr) {

#endif
	/* should be done in driver initialization */
	initConsoleDriver();
	printk("ZS OS 1.0 Initializing\n");
	printk("----------------------\n");

	call_ctors();
	printk("Entering C++ kernel_main...\n");
#ifdef __arm__

	kernel_main(r0, r1, r2);
#else
        kernel_main(magic, addr);
#endif
	return;
}
