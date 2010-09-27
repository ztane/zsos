#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stddef.h>
#include <errno.h>

#include <sys/stat.h>

#include "linuxcall.h"

static inline _syscall3(int,    __sys_write, int, fd,const char *,buf,off_t,count);
static inline _syscall3(int,    __sys_read,  int, fd,char *,buf,off_t,count);
static inline _syscall3(int,    __sys_open,  const char *,file,int,flag,int,mode);
static inline _syscall1(int,    __sys_close, int, fd);
static inline _syscall1(int,    __sys_exit,  int, err);
static inline _syscall1(void *, __sys_brk,   void *,high)
static inline _syscall3(off_t,  __sys_lseek, int,fd,off_t,offset,int,whence)
static inline _syscall2(long,   __sys_stat , const char *, filename, struct stat *, buf)
static inline _syscall2(long,   __sys_lstat, const char *, filename, struct stat *, buf)
static inline _syscall2(long,   __sys_fstat, int         , fd      , struct stat *, buf)

// static void __sys_write_character(int character) { long __res; __asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" : "=a" (__res) : "0" (0),"ri" ((long)(character)) : "memory"); do { if ((unsigned long)(__res) >= (unsigned long)(-(128 + 1))) { errno = -(__res); __res = -1; } return (void) (__res); } while (0); };

#endif
