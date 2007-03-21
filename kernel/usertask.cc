#include <iostream>
#include <cstring>

#include "task.hh"
#include "scheduler.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

#include "tss.hh"

TssContents tssSegment __attribute__((aligned(4096)));

UserTask::UserTask(const char *name, State state, int priority) 
	: Task(name, state, priority) 
{

}

void UserTask::enable_io() {
	// set IOPL to 3...
	unsigned int *tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	tmp -= 3;	
	*tmp |= 3 << 12;
}

void UserTask::initialize(void *entry) {
	unsigned int *tmp;

	// build kernel stack
	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack)) - 4;
	kstack = (unsigned int)tmp;
	tmp --;

	*tmp -- = USER_DATA_DESCRIPTOR + 3;
	*tmp -- = (unsigned int)user_stack + 16380;
        *tmp -- = 0x0202;

        *tmp -- = USER_CODE_DESCRIPTOR + 3;
        *tmp -- = (unsigned int) entry; 

        *tmp -- = USER_DATA_DESCRIPTOR + 3; // ds
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // es
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // fs
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // gs

        *tmp -- = 0;    // ebp
        *tmp -- = 0;    // esp
        *tmp -- = 0;    // edi
        *tmp -- = 0;    // esi
        *tmp -- = 0;    // edx
        *tmp -- = 0;    // ecx
        *tmp -- = 0;    // ebx
        *tmp    = 0;    // eax

	padding = 0xDEADBEEF;
	padding2 = 0xDEADBEEF;
	esp = (unsigned int)tmp;
}

UserTask::~UserTask() {
}

void UserTask::dispatch(uint32_t *saved_esp) {
	tssSegment.esp0 = kstack;
        
	if (isNew) {
                isNew = false;
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

void UserTask::terminate() {
	extern Scheduler scheduler;
	setCurrentState(TERMINATED);
	scheduler.schedule();
}

void TssContents::setup() {
	memset(&tssSegment, 0, sizeof(tssSegment));
	ss0   = KERNEL_DATA_DESCRIPTOR;

        es = 
	ss = 
	ds = 
	fs = 
	gs = USER_DATA_DESCRIPTOR + 3;

        cs = USER_CODE_DESCRIPTOR + 3;

	eflags = 2;
	bitmap = 104;

	__asm__ __volatile__ ("mov %%cr3, %0" : "=a"(cr3) : );
}

void initialize_tasking() {
	tssSegment.setup();

	uint16_t tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr %0" : : "r"(tss_desc));
}

