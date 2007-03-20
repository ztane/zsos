#include <printk.h>
#include <iostream>
#include <syscall.hh>

SYSCALL(write_character)
{
//	printk("GOT: %d\n", r.ebx);
        printk("%c", r.ebx);
        SYSCALL_RETURN(0);
}

SYSCALL(hello_world)
{
	kout << "Hello World!" << endl;
	SYSCALL_RETURN(0);
}
