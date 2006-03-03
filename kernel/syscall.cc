#include <iostream>

#include "interrupt"
#include "printk.h"
#include "scheduler"
#include "syscall"

SYSCALL(BAD)
{
	kout << "OOPS... illegal syscall #" << r.eax << endl;
	SYSCALL_RETURN(-1);
}
