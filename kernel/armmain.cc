/* main.c - the entry point for the kernel */

#include <stdint.h>
#include <kernel/arch/current/atags.hh>
#include <kernel/arch/current/uart.hh>
#include <printk.h>
#include <iostream>

// kernel main function, it all begins here
extern "C" void kernel_main(uint32_t r0, uint32_t r1, uint32_t r2) {
    atag_scanner((void*)r2);

    while (1) {
	uart_putc(uart_getc());
	uart_putc('>');
    }

    kout << "Halting" << endl;
}
