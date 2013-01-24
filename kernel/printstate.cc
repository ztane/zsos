#include "kernel/printstate.hh"
#include "kernel/printk.h"
#include "kernel/arch/current/stacktrace.hh"
#include <iostream>

void print_kernel_state(Registers& r)
{

	printk("\nRegisters:\n");
#ifndef __arm__
	printk(  "EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n",
		r.eax, r.ebx, r.ecx, r.edx);
	printk(  "ESP:%08x  EBP:%08x  ESI:%08x  EDI:%08x\n",
		r.esp, r.ebp, r.esi, r.edi);
#else
	printk(  "Not supported on ARM yet\n");
#endif

	uint32_t *stack = (uint32_t*)&r;
        void *ptrs[10];
        getStackTrace(ptrs, 10);

        kout << "Stack traceback" << endl;
        for (int i = 0; i < 10; i ++) {
		if (ptrs[i] == 0) {
			break;
		}
                kout << (i + 1) << ": " << ptrs[i] << endl;
        }
}
