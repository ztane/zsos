#include <iostream>
#include <cstring>

#include "task.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

Task::Task(const char *_name) {
	esp = 0;
	strncpy(name, _name, sizeof(name));
	isNew = true;
	current_state = READY;
	current_priority = 47;
	timeslice = 0;
}

bool Task::handlePageFault(uint32_t address) {
        printk("Process %d had a pfault at %p\n", process_id, address);
        __asm__ __volatile__ ("hlt");
        return true;
}

