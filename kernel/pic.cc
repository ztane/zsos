#include <kernel/arch/current/port.h>
#include <kernel/interrupt.hh>

#define PIC1            0x20
#define PIC2            0xA0
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)
#define PIC_EOI         0x20

#define ICW1_ICW4       0x01            /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02            /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04            /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08            /* Level triggered (edge) mode */
#define ICW1_INIT       0x10            /* Initialization - required! */

#define ICW4_8086       0x01            /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02            /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08            /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C            /* Buffered mode/master */
#define ICW4_SFNM       0x10            /* Special fully nested (not) */


/* remap the PIC controller interrupts to our vectors
   rather than the 8 + 70 as mapped by default */

static void remapAndDisablePics(int pic1, int pic2)
{
        unsigned char a1, a2;

        a1 = inb(PIC1_DATA);
        io_wait();
        a2 = inb(PIC2_DATA);
        io_wait();

        outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
        io_wait();
        outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
        io_wait();

        outb(PIC1_DATA, pic1);
        io_wait();
        outb(PIC2_DATA, pic2);
        io_wait();

        outb(PIC1_DATA, 4);
        io_wait();
        outb(PIC2_DATA, 2);
        io_wait();

        outb(PIC1_DATA, ICW4_8086);
        io_wait();
        outb(PIC2_DATA, ICW4_8086);
        io_wait();

        outb(PIC1_DATA, 0xFF & ~ (1 << 2));
        io_wait();
        outb(PIC2_DATA, 0xFF);
        io_wait();
}

void initPic()
{
        remapAndDisablePics(0x20, 0x28);
}

void enableIrq(int number) {
	bool state = disableInterrupts();
	if (number > 8) {
		int mask = 1 << (number - 8);
		outb(PIC2_DATA, inb(PIC2_DATA) & ~mask);
	}
	else {
		int mask = 1 << number;
		outb(PIC1_DATA, inb(PIC1_DATA) & ~mask);
	}
	enableInterruptsIf(state);
}

void disableIrq(int number) {
	bool state = disableInterrupts();
	if (number > 8) {
		int mask = 1 << (number - 8);
		outb(PIC2_DATA, inb(PIC2_DATA) | mask);
	}
	else {
		int mask = 1 << number;
		outb(PIC1_DATA, inb(PIC1_DATA) | mask);
	}
	enableInterruptsIf(state);
}
