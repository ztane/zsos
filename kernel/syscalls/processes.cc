#include <syscall.hh>
#include <scheduler.hh>

SYSCALL(get_pid) 
{
        extern Scheduler scheduler;
        Process *task = scheduler.getCurrentTask();
        SYSCALL_RETURN(task->getProcessId());
}

SYSCALL(suspend) 
{
	// suspend process with handle given in %EBX
	SYSCALL_RETURN(-1);
}

SYSCALL(resume)
{
	SYSCALL_RETURN(-1);
}

SYSCALL(set_thread_priority)
{
	// set priority for %EBX to %ECX
	SYSCALL_RETURN(-1);
}
