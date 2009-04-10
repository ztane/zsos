#include "timer.hh"
#include <kernel/arch/current/port.h>
#include <kernel/interrupt.hh>
#include "printk.h"

#define TIMER_IRQ 0

void initialize_timer() {
	int counter = 0x1234DD / TIMER_TICKS_PER_SECOND;

	printk("Setting timer: %08x\n", counter);
	// program timer channel 0
	outb(0x43, 0x36);
	io_wait();
	outb(0x40, counter & 0xFF);
	io_wait();
	outb(0x40, counter >> 8);

	enableIrq(TIMER_IRQ);
}
