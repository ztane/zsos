// -*- C++ -*-

#ifndef SYSCALL_INCLUDED
#define SYSCALL_INCLUDED

#include <ostypes>
#include "interrupt.hh"

inline unsigned int __syscall0(unsigned int number) {
	int retv;
	__asm__ __volatile__ ("int $0x80" : "=a"(retv) : "0"(number));
	return retv;
}

inline unsigned int __syscall1(unsigned int number, unsigned int val1) {
	int retv;
	__asm__ __volatile__ ("int $0x80" : "=a"(retv) : "0"(number), "b"(val1));
	return retv;
}

inline int write_character(char character) {
	return __syscall1(0, character);
}

inline int become_io_task() {
	return __syscall0(1);
}

inline int sem_post() {
	return __syscall0(6);
}

inline int sem_wait() {
	return __syscall0(7);
}

inline int illegal_syscall() {
	return __syscall0(1000);
}

inline pid_t get_process_id() {
	return (pid_t)__syscall0(2);
}

#define SYSCALL(name)                                                   \
        extern "C" void sys_ ## name(Registers r,                       \
                unsigned int eip, unsigned int cs, unsigned int eflags)


#define SYSCALL_RETURN(value)						\
	do { r.eax = (unsigned long)(value); return; } while(0)

#endif
