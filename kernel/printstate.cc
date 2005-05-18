#include "printstate"
#include "printk.h"

void print_kernel_state(Registers& r)
{
	printk("\nRegisters:\n");
	printk(  "EAX:%x  EBX:%x  ECX:%x  EDX:%x\n",
		r.eax, r.ebx, r.ecx, r.edx);
	printk(  "ESP:%x  EBP:%x  ESI:%x  EDI:%x\n",
		r.esp, r.ebp, r.esi, r.edi);
	printk(  "CS:%x DS:%x ES:%x FS:%x GS:%x SS:%x\n",
		10, 10, 10, 10, 10, 10);
}
