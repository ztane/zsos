#include <iostream>
#include <cstring>

#include "task.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

Task::Task(const char *_name, State state, int priority) {
	esp = 0;
	strncpy(name, _name, sizeof(name));
	isNew = true;
	setCurrentState(READY);
	setCurrentPriority(priority);
	timeslice = 0;
}

bool Task::handlePageFault(uint32_t address) {
        printk("Process %d had a pfault at %p\n", process_id, address);
        __asm__ __volatile__ ("hlt");
        return true;
}

