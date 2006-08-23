// FIXME: Change this file's name to better mirror its contents.

#include <kernel/panic.hh>

// This provides a standard entry point for the compiler to reference in
// virtual tables to indicate a pure virtual function.
// Pure virtual functions have undefined behavior according to C++ standard.
// This function does not specify its behavior, but it is supposed to
// terminate the offending process.

extern "C"
void __cxa_pure_virtual(void)
{
	kernel_panic("FATAL: pure virtual method.");
}
