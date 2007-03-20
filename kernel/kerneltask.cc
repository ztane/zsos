#include <iostream>
#include <cstring>

#include "task.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

TSSContents TSS_Segment __attribute__((aligned(4096)));

KernelTask::KernelTask(const char *name) : Task(name) {

}

void KernelTask::initialize(void (*entry)(void *), void *param) {
	unsigned int *tmp;

	// build kernel stack
	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack)) - 4;
	kstack = (unsigned int)tmp;
	tmp --;

	*tmp -- = param;
	*tmp -- = entry;
}

KernelTask::~KernelTask() {
}

void KernelTask::dispatch(uint32_t *saved_esp) {
	TSS_Segment.esp0 = kstack;
        
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

void TSSContents::setup() {
	memset(&TSS_Segment, 0, sizeof(TSS_Segment));
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
	TSS_Segment.setup();

	uint16_t tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr %0" : : "r"(tss_desc));
}

