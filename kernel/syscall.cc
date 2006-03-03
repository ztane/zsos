#include <iostream>

#include "interrupt"
#include "printk.h"
#include "scheduler"
#include "syscall"

SYSCALL(write_character) 
{
        printk("%c", r.ebx);
	
	SYSCALL_RETURN(0);
}

SYSCALL(become_io_task) 
{
	extern Scheduler scheduler;
	Process *task = scheduler.getCurrentTask();
	task->enable_io();

	SYSCALL_RETURN(0);
}

SYSCALL(BAD)
{
	kout << "OOPS... illegal syscall #" << r.eax << endl;
	SYSCALL_RETURN(-1);
}
