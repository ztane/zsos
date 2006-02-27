#include "iostream"
#include "interrupt.h"
#include "printk.h"

static unsigned int sys_write_character(const Registers& r) {
        printk("%c", r.ebx);
	return 0;
}

// TODO: dispatch multiple in asm!
extern "C" C_ISR(SYS_CALL) {
	// write character...
	if (r.eax == 0) {
		r.eax = sys_write_character(r);
		return;
	}
	printk("ILLEGAL SYSTEM CALL, #%u\n", r.eax);
}

