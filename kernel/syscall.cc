#include <iostream>

#include "interrupt.hh"
#include "printk.h"
#include "scheduler.hh"
#include "syscall.hh"
#include "errnocode.hh"

SYSCALL(BAD)
{
	kout << "OOPS... illegal syscall #" << r.syscallNumber << endl;
	SYSCALL_RETURN(-(ErrnoCode)ENOSYS);
}
