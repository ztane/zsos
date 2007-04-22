#ifndef __PANIC_INCLUDED__
#define __PANIC_INCLUDED__

#ifdef __cplusplus 
extern "C" 
#endif
void kernelPanic(char *message) __attribute__ ((noreturn));

#endif // __PANIC_INCLUDED__
