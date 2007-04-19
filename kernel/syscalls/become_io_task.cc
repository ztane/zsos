#include <interrupt.hh>
#include <scheduler.hh>
#include <usertask.hh>
#include <syscall.hh>
#include <iostream>

SYSCALL(become_io_task)
{
        extern Scheduler scheduler;
	
	kout << "become io" << endl;

        UserTask *task = (UserTask*)scheduler.getCurrentTask();
        task->enable_io();

        SYSCALL_RETURN(0);
}

