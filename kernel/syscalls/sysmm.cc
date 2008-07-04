#include <syscall.hh>
#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;
SYSCALL(brk) 
{       
        Task *task = scheduler.getCurrentTask();
        kout << "SET BRK" << endl;
        kout << (void*)r.ebx << endl;
	void *rv = task->setBrk((void*)r.ebx);
        SYSCALL_RETURN((uint32_t)rv);
}
