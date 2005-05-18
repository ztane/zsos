#include <kernel/port.h>

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

static void remap_pics(int pic1, int pic2)
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

        outb(PIC1_DATA, a1);
        io_wait();
        outb(PIC2_DATA, a2);
        io_wait();
}

void init_pic()
{
        remap_pics(0x20, 0x28);
}

void enable_keyboard()
{
        outb(0x21,0xfc);
        io_wait();
        outb(0xa1,0xff);
        io_wait();
}
