// -*- C++ -*-

#ifndef SYSCALL_INCLUDED
#define SYSCALL_INCLUDED

#include <ostypes>
#include "interrupt.hh"

inline unsigned int __syscall0(volatile unsigned int number) {
	volatile unsigned int retv;
	__asm__ __volatile__ ("int $0x80" : "=a"(retv) : "a"(number));
	return retv;
}

inline unsigned int __syscall1(volatile unsigned int number, volatile unsigned int val1) {
	volatile unsigned int retv;
	__asm__ __volatile__ ("int $0x80" : "=a"(retv) : "a"(number), "b"(val1));
	return retv;
}

#define SYSCALL(name)                                                   \
        extern "C" void sys_ ## name(volatile Registers r,          	\
                unsigned int eip, unsigned int cs, unsigned int eflags)


#define SYSCALL_RETURN(value)						\
	do { 								\
		r.syscallRetval = value;				\
		return; 						\
	} while(0)

#endif
