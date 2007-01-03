#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "interrupt.hh"
#include "scheduler.hh"
#include "tasking.hh"

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
		return;
	}
	
	scheduler_running = true;
	
	while (true) {
		for (i = 0; i < PRIV_LEVELS; i++)
		{
			if (tasks[i].first != NULL) {
				Process *p = tasks[i].first;
				Process *old = current;
				remove_process(p);
								
				current = p;
				p->current_state = Process::RUNNING;
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

void Scheduler::remove_process(Process *p) {
	bool fl = disableInterrupts();
	
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
	
	enableInterruptsIf(fl);
}

void Scheduler::inc_ticks() {
	bool fl = disableInterrupts();
	
	n_ticks ++;
	current->timeslice ++;
	if (current->timeslice > 0) {
		current->timeslice = 0;
		if (current->current_state == Process::RUNNING)
			add_process(current);
	}
	
	enableInterruptsIf(fl);
}

void Scheduler::add_process(Process *p)
{
	bool fl = disableInterrupts();
	
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
	
	enableInterruptsIf(fl);
}

void Scheduler::dispatchNew(Process *old, Process *nu) {
	uint32_t *esp_ptr;
	uint32_t dummy;

	if (old == NULL)
		esp_ptr = &dummy;
	else
		esp_ptr = &(old->esp);
	
        TSS_Segment.esp0 = nu->kstack;

	if (nu->isNew) {
		nu->isNew = false;
		__asm__ __volatile__ (
			"pushal\n\t"
			"mov %%esp, (%1)\n\t"
                	"mov %0, %%esp\n\t"
                	"popal\n\t"
                	"pop %%gs\n\t"
                	"pop %%fs\n\t"
                	"pop %%es\n\t"
                	"pop %%ds\n\t"
                	"iret"
                	: : "a"(nu->esp), "b"(esp_ptr));
	}
	else {
	        __asm__ __volatile__ (
        	        "pushal\n\t"
               		"mov %%esp, (%1)\n\t"
               		"mov %0, %%esp\n\t"
                	"popal\n\t"
                	:
                	: "a"(nu->esp), "b"(&(old->esp)));
	}
}
