#include "interrupt.h"
#include "printk.h"

extern "C" C_ISR(SYS_CALL) {
        printk("%c", r.eax);
}

