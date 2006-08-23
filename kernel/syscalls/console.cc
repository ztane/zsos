#include <printk.h>
#include <syscall.hh>

SYSCALL(write_character)
{
        printk("%c", r.ebx);

        SYSCALL_RETURN(0);
}


