#include <printk.h>
#include <syscall.hh>

SYSCALL(write_character)
{
//	printk("GOT: %d\n", r.ebx);
        printk("%c", r.ebx);
        SYSCALL_RETURN(0);
}


