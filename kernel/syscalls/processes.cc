#include <syscall>
#include <scheduler>

SYSCALL(get_pid) 
{
	// return pid...
	SYSCALL_RETURN(0);
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
