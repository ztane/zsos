#include <iostream>
#include <kernel/printk.h>

ostream kout;

ostream& ostream::operator<<(const char* parm) 
{
	printk("%s", parm);
        return *this;
}

ostream& ostream::operator<<(int parm)
{
        printk("%d", parm);
        return *this;
}

ostream& ostream::operator<<(unsigned int parm)
{
        printk("%u", parm);
        return *this;
}

const char *endl = "\n";
