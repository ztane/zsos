#include <interrupt.hh>
#include <scheduler.hh>
#include <usertask.hh>
#include <syscall.hh>

SYSCALL(become_io_task)
{
        extern Scheduler scheduler;
        UserTask *task = (UserTask*)scheduler.getCurrentTask();
        task->enable_io();

        SYSCALL_RETURN(0);
}

