// FIXME: Change this file's name to better mirror its contents.

#include <kernel/printk.h>

// This will get called only if the user calls a non-overridden pure
// virtual function, which has undefined behaviour according to the C++
// Standard -- hence we terminate.

extern "C" void __cxa_pure_virtual();
void __cxa_pure_virtual()
{
	printk("WARNING: attempt to use pure virtual function!\n");
	printk("PANIC!");

	while (1)
		__asm__ __volatile__ ("hlt");
}
