#include "printstate.hh"
#include "printk.h"

void print_kernel_state(Registers& r)
{
	printk("\nRegisters:\n");
	printk(  "EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n",
		r.eax, r.ebx, r.ecx, r.edx);
	printk(  "ESP:%08x  EBP:%08x  ESI:%08x  EDI:%08x\n",
		r.esp, r.ebp, r.esi, r.edi);
	//printk(  "CS:%04x DS:%04x ES:%04x FS:%04x GS:%04x SS:%04x\n",
	//	10, 10, 10, 10, 10, 10);
}
