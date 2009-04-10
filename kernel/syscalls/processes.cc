#include <syscall.hh>
#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;
SYSCALL(get_pid) 
{       
        Task *task = scheduler.getCurrentTask();
        SYSCALL_RETURN(task->getProcessId());
}

SYSCALL(suspend) 
{
	// suspend process with handle given in %EBX
	SYSCALL_RETURN(0xFFFFFFFF);
}

SYSCALL(exit)
{
        Task *task = scheduler.getCurrentTask();
	task->terminate();
	__asm__ __volatile__ ("cli; hlt;");
	SYSCALL_RETURN(0xFFFFFFFF);
}

SYSCALL(resume)
{
	SYSCALL_RETURN(0xFFFFFFFF);
}

SYSCALL(set_thread_priority)
{
	// set priority for %EBX to %ECX
	SYSCALL_RETURN(0xFFFFFFFF);
}

#include "semaphore.hh"
#include "ringbuffer.hh"
static Semaphore<int32_t> testSem;

extern RingBuffer<int>* buf;

SYSCALL(sem_post)
{
	buf->put(r.ebx);
	SYSCALL_RETURN(0);
}

SYSCALL(sem_wait) 
{
	int rv;
	buf->get(rv);
	SYSCALL_RETURN(rv);
}
