#include <iostream>
#include <cstring>
#include "kernel/printk.h"

#include "kernel/kerneltask.hh"

#include "kernel/interrupt.hh"
#include "kernel/memory.hh"
#include "kernel/panic.hh"
#include "kernel/scheduler.hh"
#include "kernel/task.hh"

KernelTask::KernelTask(const char *name, State state, int priority)
	: Task(name, state, priority)
{
	kernelStackSize = 4096;
}

static void do_dispatch_kernel_task(void (*entry)(void *), void *param);

void KernelTask::initialize(void (*entry)(void *), void *param) {
	unsigned int *tmp;

	// build kernel stack
	kernelStack = new uint8_t[kernelStackSize];
	tmp = (unsigned int *)(kernelStack + kernelStackSize);
	kstack = (unsigned int)tmp;
	tmp --;

	*tmp -- = (uint32_t)param;
	*tmp -- = (uint32_t)entry;

	// dummy placeholder for function return address...
	*tmp -- = (uint32_t)0xdeadbeef;

	// ugly hack: return to function entry point ... ;)
	*tmp -- = (uint32_t)do_dispatch_kernel_task;

	// general purpose registers
	for (int i = 0; i < 8; i ++)
		*tmp -- = i + 1;

        esp = (uint32_t)(tmp + 1);
}

KernelTask::~KernelTask() {
}

// this function is never called, instead the entry is RETURNED to...
static void do_dispatch_kernel_task(void (*entry)(void *), void *param) {
	extern Scheduler scheduler;

	__enableInterrupts();

	entry(param);
	scheduler.getCurrentTask()->terminate();

	kernelPanic("return from scheduler->terminate()?!");
}

void KernelTask::terminate() {
	extern Scheduler scheduler;

	// no need to save..
	disableInterrupts();
	scheduler.removeTask(this);
	setCurrentState(TERMINATED);

	// todo: delete elsewhere...
	// delete[] kernelStack;

	// will never return

	scheduler.schedule();
	kernelPanic("Returned to a terminated task!!");
}

void KernelTask::dispatch(uint32_t *saved_esp) {
	switchContexts(saved_esp);
}
