#include "tasking"
#include "memory"
#include "string.h"
#include "iostream"

TSSContents TSS_Segment __attribute__((aligned(4096)));

void TSSContents::setup() {
	backlink = __blh = __ss0h = 0;
        esp0  = esp1   = esp2 = 0;
        ss1   = __ss1h = ss2  = __ss2h = 0;

	ss0   = KERNEL_DATA_DESCRIPTOR;

	__asm__ __volatile__ ("mov %%cr3, %0" : "=a" (cr3) : );

	eip = eflags = eax = ecx = edx = 
        ebx =    esp = ebp = esi = edi = 0;
       
        es = ss = ds = fs = gs = USER_DATA_DESCRIPTOR + 3;
        cs = USER_CODE_DESCRIPTOR + 3;

        ldt = trace = bitmap = 0;
}

Process::Process(char *_name) {
	esp = 0;
	strncpy(name, _name, sizeof(name));
}

char user_stack[16384] __attribute__((aligned(4096)));

void Process::initialize(void *entry) {
	unsigned int *tmp;

	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	tmp --;

	*tmp -- = USER_DATA_DESCRIPTOR + 3;
	*tmp -- = (unsigned int)user_stack + 16380;
        *tmp -- = 0x0202;
        *tmp -- = USER_CODE_DESCRIPTOR + 3;
        *tmp -- = (unsigned int) entry; 
        *tmp -- = 0;    // ebp
        *tmp -- = 0;    // esp
        *tmp -- = 0;    // edi
        *tmp -- = 0;    // esi
        *tmp -- = 0;    // edx
        *tmp -- = 0;    // ecx
        *tmp -- = 0;    // ebx
        *tmp -- = 0;    // eax
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // ds
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // es
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // fs
        *tmp    = USER_DATA_DESCRIPTOR + 3; // gs

	esp  = (unsigned int)tmp;
}

void Process::dispatch() {
	
	kout << "Dispatching..." << endl;
	__asm__ __volatile__ (
		"mov (%0), %%esp\n\t"
		"mov %%ebx, TSS_Segment+4\n\t"
		"pop %%gs\n\t"
		"pop %%fs\n\t"
		"pop %%es\n\t"
		"pop %%ds\n\t"
		"popa\n\t"
		"hlt\n\t"
		"hlt\n\t"
		"iret"
		: : "a"(this));
}

void initialize_tasking() {
	TSS_Segment.setup();

	unsigned short tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr (%0)" : : "r" (&tss_desc));
}

