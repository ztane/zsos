#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "interrupt.hh"
#include "scheduler.hh"
#include "task.hh"
#include "printk.h"
#include "bottomhalves.hh"

Scheduler::Scheduler()
{
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
	int i;
	
	// unconditionally they MUST be disabled here...
	bool fl = disableInterrupts();
	if (scheduler_running) {
		enableInterruptsIf(fl);
		return;
	}
	scheduler_running = true;
	
	while (true) {
		for (i = 0; i < PRIV_LEVELS; i++)
		{
			if (tasks[i].first != NULL) {
				Task *p = tasks[i].first;
				Task *old = current;
				remove_task(p);
								
				current = p;
				p->current_state = Task::RUNNING;
				scheduler_running = false;
				dispatchNew(old, p);

				enableInterruptsIf(fl);
				return;		
			}
		}
	
		// and unconditionally enabled before halt (no shit?)
		enableInterruptsIf(true);
		__asm__ __volatile__ (
			"hlt\t\n"
		::);
	}
}

void Scheduler::remove_task(Task *p) {
	bool fl = disableInterrupts();
	
	task_dir *entry = &tasks[p->getCurrentPriority()];
	
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
	
	enableInterruptsIf(fl);
}

void Scheduler::inc_ticks() {
	bool fl = disableInterrupts();
	
	n_ticks ++;
	current->timeslice ++;
	if (current->timeslice > 0) {
		current->timeslice = 0;
		if (current->current_state == Task::RUNNING)
			add_task(current);
	}
	
	enableInterruptsIf(fl);
}

void Scheduler::add_task(Task *p)
{
	bool fl = disableInterrupts();
	
	int prio;
	Task *secondlast;
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
	p->current_state = Task::READY;
	
	enableInterruptsIf(fl);
}

void Scheduler::dispatchNew(Task *old, Task *nu) {
	uint32_t *esp_ptr;
	uint32_t dummy;

	if (old == nu)
		return;	

	if (old == NULL)
		esp_ptr = &dummy;
	else
		esp_ptr = &(old->esp);
	
	nu->dispatch(esp_ptr);
}

void Scheduler::needsScheduling() {
	markBottomHalf(BH_SCHEDULER);
}

void schedulerBottomHalf() {
	scheduler.schedule();
}
