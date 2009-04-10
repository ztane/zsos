#ifndef __PORT_H_INCLUDED__
#define __PORT_H_INCLUDED__

static __inline__ void outb(unsigned short port, unsigned char val)
{
   asm volatile("outb %0,%1"::"a"(val), "Nd"(port));
}

static __inline__ unsigned char inb(unsigned short port)
{
   unsigned char ret;
   asm volatile ("inb %1,%0":"=a"(ret):"Nd"(port));
   return ret;
}

static __inline__ void outw(unsigned short port, unsigned short val)
{
   asm volatile("outw %0,%1"::"a"(val), "Nd"(port));
}

static __inline__ unsigned short inw(unsigned short port)
{
   unsigned short ret;
   asm volatile ("inw %1,%0":"=a"(ret): "Nd"(port));
   return ret;
}

static __inline__ void outl(unsigned short port, unsigned int val)
{
   asm volatile("outl %0,%1"::"a"(val), "Nd"(port));
}

static __inline__ unsigned int inl(unsigned short port)
{
   unsigned int ret;
   asm volatile ("inl %1,%0":"=a"(ret):"Nd"(port));
   return ret;
}

// Use I/O cycle on an 'unused' port (which has the nice property
// of being CPU-speed independent): port 0x80 is used for 'checkpoints' 
// during POST. Linux kernel seems to think it's free for use :-/
static __inline__ void io_wait(void)
{
   asm volatile("outb %al, $0x80");
}

static __inline__ void unlock_irq(int number)
{
    // unlock both?
    if (number >= 8)
    {
        outb(0x20,0xa0);
    }
    outb(0x20,0x20);
}

#endif
