#include "timer.hh"
#include "port.h"
#include "printk.h"

void initialize_timer() {
	int counter = 0x1234DD / TIMER_TICKS_PER_SECOND;

	printk("Setting timer: %08x\n", counter);
	// program timer channel 0
	outb(0x43, 0x36);
	io_wait();
	outb(0x40, counter & 0xFF);
	io_wait();
	outb(0x40, counter >> 8);
}
