#include "kernel/kerneltask.hh"
#include "kernel/scheduler.hh"
#include <iostream>

static KernelTask idleTask("idletask");

void idleTaskRoutine(void *parm) {
	while (1) {
		kout << "idling..." << endl;
		__asm__ __volatile__ ("hlt");
	}
}

extern Scheduler scheduler;

void startIdleTask() {
	idleTask.initialize(idleTaskRoutine, 0);
	idleTask.setCurrentPriority(48);
	scheduler.addTask(&idleTask);
}

