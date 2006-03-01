#include <iostream>

#include "interrupt"
#include "printk.h"
#include "scheduler"

static unsigned int sys_write_character(const Registers& r) {
        printk("%c", r.ebx);
	return 0;
}

static unsigned int sys_become_io_task(const Registers& r) {
	extern Scheduler scheduler;
	Process *task = scheduler.getCurrentTask();
	task->enable_io();
	return 0;
}

// TODO: dispatch multiple in asm!
extern "C" C_ISR(SYS_CALL) {
	// write character...
	if (r.eax == 0) {
		r.eax = sys_write_character(r);
		return;
	}
	if (r.eax == 1) {
		r.eax = sys_become_io_task(r);
		return;
	}
	printk("ILLEGAL SYSTEM CALL, #%u\n", r.eax);
}

