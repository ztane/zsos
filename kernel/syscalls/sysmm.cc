#include <syscall.hh>
#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;
SYSCALL(brk)
{
        Task *task = scheduler.getCurrentTask();
	void *rv = task->setBrk((void*)r.arg0);
        SYSCALL_RETURN((uint32_t)rv);
}
