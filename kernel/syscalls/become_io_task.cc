#include <interrupt>
#include <scheduler>
#include <syscall>

SYSCALL(become_io_task)
{
        extern Scheduler scheduler;
        Process *task = scheduler.getCurrentTask();
        task->enable_io();

        SYSCALL_RETURN(0);
}

