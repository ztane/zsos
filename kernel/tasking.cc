#include <iostream>
#include <cstring>

#include "tasking.hh"
#include "memory.hh"
#include "printk.h"

TSSContents TSS_Segment __attribute__((aligned(4096)));

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

	__asm__ __volatile__ ("mov %%cr3, %0" : "=a" (cr3) : );
}

Process::Process(char *_name) {
	esp = 0;
	strncpy(name, _name, sizeof(name));

	current_state = READY;
	current_priority = 47;
	timeslice = 0;
}

void Process::enable_io() {
	// set IOPL to 3...
	unsigned int *tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	tmp -= 3;	
	*tmp |= 3 << 12;
}

void Process::initialize(void *entry) {
	unsigned int *tmp;

	// build kernel stack
	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
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

	esp = (unsigned int)tmp;
}

void Process::dispatch() {
	TSS_Segment.esp0 = kstack;
	__asm__ __volatile__ (
		"mov %0, %%esp\n\t"
		"popal\n\t"
		"pop %%gs\n\t"
		"pop %%fs\n\t"
		"pop %%es\n\t"
		"pop %%ds\n\t"
		"iret"
		: : "a"(this->esp));
}

void initialize_tasking() {
	TSS_Segment.setup();

	unsigned short tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr (%0)" : : "r" (&tss_desc));
}

