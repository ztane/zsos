#include "kernel/syscall.hh"
#include "kernel/initial_vga.h"

#include <iostream>

SYSCALL(write_character)
{
//	printk("GOT: %d\n", r.ebx);
//	printk("%c", r.ebx);
        SYSCALL_RETURN(0);
}

SYSCALL(write) 
{
	vga_buf_write((void*)r.ecx, r.edx);
        SYSCALL_RETURN(0);	
}

SYSCALL(hello_world)
{
	SYSCALL_RETURN(0);
}
