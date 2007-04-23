#include <iostream>
#include <cstring>

#include "kernel/task.hh"
#include "kernel/usertask.hh"
#include "kernel/memory.hh"
#include "kernel/printk.h"
#include "kernel/panic.hh"

Task::Task(const char *_name, State state, int priority) {
	esp = 0;
	strncpy(name, _name, sizeof(name));
	isNew = true;
	setCurrentState(READY);
	setCurrentPriority(priority);
	timeslice = 0;
	memmap = 0;
}

bool Task::handlePageFault(PageFaultInfo& f) {
	kernelPanic("Page fault in a non-user task!");
        return true;
}

void Task::switchContexts(uint32_t *saved_esp) {
        __asm__ __volatile__ (
                "call 1f\n\t"
                "jmp 2f\n"
                "1:\n\t"
                "pushal\n\t"
                "mov %%esp, (%1)\n\t"
                "mov %0, %%esp\n\t"
                "popal\n\t"
                "ret\n"
                "2:\n\t"
                : : "a"(esp), "b"(saved_esp));
}
