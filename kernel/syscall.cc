#include <iostream>

#include "interrupt.hh"
#include "printk.h"
#include "scheduler.hh"
#include "syscall.hh"

SYSCALL(BAD)
{
	kout << "OOPS... illegal syscall #" << r.eax << endl;
	SYSCALL_RETURN(-1);
}
