// FIXME: Change this file's name to better mirror its contents.

#include <kernel/printk.h>

// This provides a standard entry point for the compiler to reference in
// virtual tables to indicate a pure virtual function.
// Pure virtual functions have undefined behavior according to C++ standard.
// This function does not specify its behavior, but it is supposed to
// terminate the offending process.

extern "C"
void __cxa_pure_virtual(void)
{
	printk("FATAL: pure virtual method.\n");
	// Do something to terminate here.
	// Even if it's the kernel.
}
