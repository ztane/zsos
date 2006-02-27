#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "scheduler.hh"
#include "tasking"

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
/*
Scheduler::~Scheduler()
{
	kout << "Scheduler: YARR!! They killed me!" << endl;
}
*/
void Scheduler::schedule()
{
	int idx;
	process_dir *p_dir = NULL;
	
	for (idx = 0; idx < PRIV_LEVELS; idx++)
	{
		if (tasks[idx].next->process != NULL)
		{
			p_dir = tasks[idx].next;
			
			tasks[idx].next = tasks[idx].next->next;
			tasks[idx].next->prev = &tasks[idx];
			
			tasks[idx].prev->next = p_dir;
			p_dir->prev = tasks[idx].prev;
			p_dir->next = &tasks[idx];
			tasks[idx].prev = p_dir;
			
			kout << "Scheduler: dispatching..." << endl;
			
			current = p_dir->process;
			current->dispatch();
		}
	}
}

void Scheduler::add_process(Process *p, unsigned int priv)
{
	process_dir *p_dir = (process_dir *) kmalloc(sizeof(process_dir));
	
	if (!p_dir)
	{
		kout << "Scheduler: add_process: kmalloc returned NULL" << endl;
		return;
	}
	
	p_dir->process = p;
	p_dir->prev = &tasks[priv];
	p_dir->next = tasks[priv].next;
	tasks[priv].next->prev = p_dir;
	tasks[priv].next = p_dir;
}
