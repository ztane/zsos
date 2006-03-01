#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "interrupt"
#include "scheduler"
#include "tasking"

Scheduler::Scheduler()
{
	kout << "Scheduler: setting up process queues" << endl;
	
	scheduler_running = false;
	for (int i = 0; i < PRIV_LEVELS; i++)
	{
		tasks[i].first = NULL;
		tasks[i].last = NULL;
	}
	
	n_ticks = 0;
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
	
	enter_critical();
	if (scheduler_running) {
		return;
	}
	
	scheduler_running = true;
	
	while (true) {
		for (idx = 0; idx < PRIV_LEVELS; idx++)
		{
			if (tasks[idx].first != NULL) {
				Process *p = tasks[idx].first;
				remove_process(p);
				
				current = p;
				p->current_state = Process::RUNNING;
				scheduler_running = false;
				
				__critical_nest_depth = 0;
				p->dispatch();
			}
		}
	
		leave_critical();
		__asm__ __volatile(
			"hlt\t\n"
		::);
	}
}

void Scheduler::remove_process(Process *p) {
	enter_critical();
	
	process_dir *entry = &tasks[p->getCurrentPriority()];
	
	if (p->next) {
		p->next->previous = p->previous;
	} 
	else {
		entry->last = p->previous;
	}
	
	if (p->previous) {	
		p->previous->next = p->next;
	}
	else {
		entry->first = p->next;
	}
	p->next = p->previous = NULL;
	
	leave_critical();
}

void Scheduler::inc_ticks() {
	enter_critical();
	
	n_ticks ++;
	current->timeslice ++;
	if (current->timeslice > 0) {
		current->timeslice = 0;
		add_process(current);
	}
	
	leave_critical();
}

void Scheduler::add_process(Process *p)
{
	enter_critical();
	
	int prio;
	Process *secondlast;
	prio = p->getCurrentPriority();
	secondlast = tasks[prio].last;
	p->next = p->previous = NULL;
	if (secondlast) {
		p->previous = secondlast;
		secondlast->next = p;
	}
	else {
		tasks[prio].first = p;
	}
	tasks[prio].last = p;
	p->current_state = Process::READY;
	
	leave_critical();
}
