#include <printk.h>
#include <iostream>

extern "C" void kernel_panic(char *message) 
{
	kout << "KERNEL PANIC!!!" << endl;
	kout << message;

        while(1) {
                __asm__ __volatile__("cli;\n\thlt");
        }
}
