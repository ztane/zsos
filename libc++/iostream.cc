#include <iostream>
#include <kernel/printk.h>

ostream kout;

ostream& ostream::operator<<(const char* parm) 
{
	printk("%s", parm);
        return *this;
}

ostream& ostream::operator<<(const void* parm) 
{
	printk("%p", parm);
        return *this;
}

ostream& ostream::operator<<(short parm) {
	return *this << (int)parm;
}

ostream& ostream::operator<<(unsigned short parm) {
	return *this << (unsigned int)parm;
}

ostream& ostream::operator<<(long parm) {
	return *this << (int)parm;
}

ostream& ostream::operator<<(unsigned long parm) {
	return *this << (unsigned int)parm;
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

ostream& ostream::operator<<(unsigned char parm)
{
        printk("%c", parm);
        return *this;
}

ostream& ostream::operator<<(signed char parm)
{
        printk("%c", parm);
        return *this;
}

const char *endl = "\n";
