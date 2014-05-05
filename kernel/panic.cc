#include <printk.h>
#include <iostream>
#include <kernel/arch/current/halt.hh>
#include <kernel/arch/current/interrupt.hh>

extern "C" void kernelPanic(const char *message)
{
	kout << "KERNEL PANIC!!!" << endl;
	kout << message;

        while(1) {
		disableInterrupts();
		halt();
        }
}
