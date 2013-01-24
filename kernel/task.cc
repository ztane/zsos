#include <iostream>
#include <cstring>

#include "kernel/arch/current/contextswitch.hh"
#include "kernel/task.hh"
#include "kernel/usertask.hh"
#include "kernel/memory.hh"
#include "kernel/printk.h"
#include "kernel/panic.hh"
#include "kernel/fs/filedescriptor.hh"

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

void *Task::setBrk(void *newBrk) {
	return 0;
}

void Task::switchContexts(uint32_t *saved_esp) {
        doContextSwitch(esp, saved_esp);
}

FileDescriptor *Task::getFileDescriptor(int number) {
         if (number < 0 || number >= MAX_FILEDES) {
                  return NULL;
         }

         return fileDescriptors[number];
}

bool Task::setFileDescriptor(int number, FileDescriptor* value) {
        if (number < 0 || number >= MAX_FILEDES) {
                  return false;
        }

        fileDescriptors[number] = value;
	return true;
}

int Task::findFreeFdSlot(FileDescriptor* value) {
	for (int i = 0; i < MAX_FILEDES; i++) {
		if (! fileDescriptors[i]) {
			fileDescriptors[i] = value;
			return i;
		}
	}

	return -1;
}

void Task::handleNMException() {
    kernelPanic("Non-user task got FPU not present exception");
}
