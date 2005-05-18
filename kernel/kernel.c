#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chardev.h"
#include "printk.h"

extern char_device_iface vga;

static char *heap_mem[4096];

extern void init_vga_buffer();

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
	/* Tämän pitäis tapahtua ajurin latauksessa!! */
	init_vga_buffer();
	printk("ZS OS 1.0 Initializing\n");
	printk("Setting up memory arena...");
	kmalloc_init(heap_mem, sizeof(heap_mem));
	printk(" done.\n");

	call_ctors();
	printk("Entering C++ kernel_main...\n");
	kernel_main(magic, addr);

	return;
}
