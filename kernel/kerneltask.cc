#include <iostream>
#include <cstring>

#include "scheduler.hh"
#include "task.hh"
#include "kerneltask.hh"
#include "memory.hh"
#include "printk.h"

KernelTask::KernelTask(const char *name, State state, int priority) 
	: Task(name, state, priority) 
{

}

void KernelTask::initialize(void (*entry)(void *), void *param) {
	unsigned int *tmp;

	// build kernel stack
	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack)) - 4;
	kstack = (unsigned int)tmp;
	tmp --;

	*tmp -- = (unsigned int)param;
	*tmp    = (unsigned int)entry;

        esp = (unsigned int)tmp;
}

KernelTask::~KernelTask() {
}

extern "C" static void do_dispatch_kernel_task(void (*entry)(void *), void *param) {
	extern Scheduler scheduler;

	entry(param);
	scheduler.getCurrentTask()->terminate();
}

void KernelTask::terminate() {
	extern Scheduler scheduler;

	setCurrentState(TERMINATED);
	scheduler.schedule();	
}

void KernelTask::dispatch(uint32_t *saved_esp) {
	if (isNew) {
                isNew = false;
                __asm__ __volatile__ (
                        "pushal\n\t"
                        "mov %%esp, (%1)\n\t"
                        "mov %0, %%esp\n\t"
			// now would be a good time to enable interrupts (ASAP).
			"sti\n\t"
                        "call do_dispatch_kernel_task"
                        : : "a"(esp), "b"(saved_esp));
        }
        else {
                __asm__ __volatile__ (
                        "pushal\n\t"
                        "mov %%esp, (%1)\n\t"
                        "mov %0, %%esp\n\t"
                        "popal\n\t"
                        : : "a"(esp), "b"(saved_esp));
        }
}
