#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.hh"

Scheduler::Scheduler()
{
	kout << "Scheduler: setting up process queues" << endl;
	
	for (int i = 0; i < PRIV_LEVELS; i++)
	{
		tasks[i].process = NULL;
		tasks[i].next    = &tasks[i];
		tasks[i].prev    = &tasks[i];
	}
}

Scheduler::~Scheduler()
{
	kout << "Scheduler: YARR!! They killed me!" << endl;
}

Scheduler::schedule()
{
	int idx;
	Process *p = NULL;
	process_dir *p_dir = NULL;
	
	for (idx = 0; idx < PRIV_LEVELS; idx++)
	{
		if (tasks[idx].next->process != NULL)
		{
			p     = tasks[idx].next->process;
			p_dir = tasks[idx].next;
			
			tasks[idx].next = tasks[idx].next->next;
			tasks[idx].next->prev = &tasks[idx];
			
			tasks[idx].prev->next = p_dir;
			p_dir->prev = tasks[idx].prev;
			p_dir->next = &tasks[idx];
			tasks[idx].prev = p_dir;
			
			current = p;
			p->dispatch();
		}
	}

	return;
}

Scheduler::add_process(Process *p, unsigned int priv)
{
	return;
}
