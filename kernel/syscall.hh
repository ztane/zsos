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

inline int write_character(char character) {
	return __syscall1(0, character);
}

inline int become_io_task() {
	return __syscall0(1);
}

inline int sem_post(int c) {
	return __syscall1(6, c);
}

inline int sem_wait() {
	return __syscall0(7);
}

inline int hello_world() {
	return __syscall0(8);
}

inline int illegal_syscall() {
	return __syscall0(1000);
}

inline pid_t get_process_id() {
	return (pid_t)__syscall0(2);
}

#define SYSCALL(name)                                                   \
        extern "C" void sys_ ## name(volatile Registers r,          	\
                unsigned int eip, unsigned int cs, unsigned int eflags)


#define SYSCALL_RETURN(value)						\
	do { 								\
		r.eax = value;						\
		return; 						\
	} while(0)

#endif
