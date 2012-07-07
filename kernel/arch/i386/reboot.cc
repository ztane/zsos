#include <kernel/arch/i386/port.h>
#include <kernel/interrupt.hh>
#include <kernel/syscall.hh>
#include <kernel/arch/current/halt.hh>

void reboot() {
	int counter;
	disableInterrupts();

	// try the keyboard method
	for (counter = 0x10000; counter && inb(0x64) & 2; counter --);
	outb(0x64, 0xD1);

	for (counter = 0x10000; counter && ! (inb(0x64) & 2); counter --);
	outb(0x60, 0xFE);

	// generate a triple fault by first loading an invalid IDT (limit 0)
	load_idt(0, 0);

	// and then call exit...
	__syscall0(1);

	// and if all else fails, halt.
	halt();
}
