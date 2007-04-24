#ifndef __UNISTD_H__
#define __UNISTD_H__

#define ENOMEM -1

#include <sys/syscall.h>
#include <stddef.h>

int brk(void *__n);
void *sbrk(ptrdiff_t __i);

#endif
