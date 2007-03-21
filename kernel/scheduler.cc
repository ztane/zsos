#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "kernel/panic.hh"
#include "interrupt.hh"
#include "scheduler.hh"
#include "task.hh"
#include "printk.h"
#include "bottomhalves.hh"

Scheduler::Scheduler()
{
	for (int i = 0; i < PRIV_LEVELS; i++)
	{
		tasks[i].first = NULL;
		tasks[i].last = NULL;
	}
	
	nTicks = 0;
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
	for (i = 0; i < PRIV_LEVELS; i++)
	{
		if (tasks[i].first != NULL) {
			Task *p = tasks[i].first;
			Task *old = current;
			removeTask(p);
			addTask(p);				
			current = p;
			p->current_state = Task::RUNNING;

			dispatchNew(old, p);
			enableInterruptsIf(fl);
			return;		
		}
	}

	kernelPanic("NO TASKS TO SCHEDULE!");	
}

void Scheduler::removeTask(Task *p) {
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

void Scheduler::incTicks() {
	bool fl = disableInterrupts();
	
	nTicks ++;
	current->timeslice ++;
	if (current->timeslice > 0) {
		current->timeslice = 0;
		if (current->current_state == Task::RUNNING) {
			removeTask(current); 
			addTask(current);
		}
	}
	
	enableInterruptsIf(fl);
}

void Scheduler::addTask(Task *p)
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
}

void schedulerBottomHalf() {
}
