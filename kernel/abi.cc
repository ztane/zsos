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
	kernelPanic("FATAL: pure virtual method.");
}

extern "C"
int __cxa_atexit(void (*fp)(void *), void *vp1, void *vp2)
{
	return 0;
}


// extern "C" void __stack_chk_fail(void)
// {	
//	kernelPanic("stack-protector: Kernel stack is corrupted");
// }

void *__dso_handle = &__dso_handle;
