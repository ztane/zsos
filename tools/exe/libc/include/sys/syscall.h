#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stddef.h>
#include <errno.h>

static ssize_t write(int p1,const void* p2,size_t p3) { long __res; __asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" : "=a" (__res) : "0" (4),"ri" ((long)(p1)),"c" ((long)(p2)), "d" ((long)(p3)) : "memory"); do { if ((unsigned long)(__res) >= (unsigned long)(-(128 + 1))) { errno = -(__res); __res = -1; } return (ssize_t) (__res); } while (0); };

static void exit(int err) { long __res; __asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" : "=a" (__res) : "0" (1),"ri" ((long)(err)) : "memory"); do { if ((unsigned long)(__res) >= (unsigned long)(-(128 + 1))) { errno = -(__res); __res = -1; } return (void) (__res); } while (0); };

static void *__sys_brk(void * newad) { long __res; __asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" : "=a" (__res) : "0" (45),"ri" ((long)(newad)) : "memory"); do { if ((unsigned long)(__res) >= (unsigned long)(-(128 + 1))) { errno = -(__res); __res = -1; } return (void *) (__res); } while (0); };

#endif
