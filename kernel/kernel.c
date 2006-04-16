#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mm/kmalloc.h"
#include "initial_vga.h"
#include "printk.h"

extern void (*__CTOR_LIST__)();

static void call_ctors() 
{
    void (**fptr)() = (&__CTOR_LIST__) + 1;
    int count = ((int *)(&__CTOR_LIST__))[0];
    printk("Executing %d static C++ initializers...", count);

    while (count--)
    {
        (*fptr++)();
    }

    printk(" done\n");
}

extern void kernel_main(unsigned long, void *);

void kmain(unsigned int magic, void *addr) {
	/* should be done in driver initialization */
	init_vga_buffer();
	printk("ZS OS 1.0 Initializing\n");
	printk("----------------------\n");

	call_ctors();
	printk("Entering C++ kernel_main...\n");
	kernel_main(magic, addr);

	return;
}
