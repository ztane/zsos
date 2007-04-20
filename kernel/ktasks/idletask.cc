#include "kernel/kerneltask.hh"
#include "kernel/scheduler.hh"
#include "kernel/arch/current/halt.hh"
#include <iostream>

void idleTaskRoutine(void *parm) {
	while (1) {
		// do something useful here first...
		// everything done, it'd be a good time to hlt..
	
		// kout << "idling..." << endl;
		halt();
		// resume after interrupt!
	}
}

extern Scheduler scheduler;

static KernelTask idleTask("idletask", Task::READY, Task::IDLE);

void startIdleTask() {
	idleTask.initialize(idleTaskRoutine, 0);
	idleTask.setProcessId(10);
	scheduler.addTask(&idleTask);
}

