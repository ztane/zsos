#include <cstdlib>
#include "config.h"
#include "interrupt.hh"
#include "port.h"
#include "pokepeek.h"
#include "printk.h"
#include "printstate.hh"
#include "tasking.hh"
#include "scheduler.hh"
#include "timer.hh"

#define ASM_ISR(name) 			\
	void __ISR_ ## name ## _asm();	\

#define ISR_PROTOS(name) 	 \
	C_ISR(name);	 	 \
	ASM_ISR(name);   	 \

#define ISR_PROTOS_W_ECODE(name) \
	C_ISR_W_ECODE(name);	 \
	ASM_ISR(name);           \


extern "C" { 
	ISR_PROTOS(divide_by_zero)
	ISR_PROTOS(debug_exception)
	ISR_PROTOS(nmi)
	ISR_PROTOS(breakpoint)
	ISR_PROTOS(overflow)
	ISR_PROTOS(bound_exception)
	ISR_PROTOS(invalid_opcode)
	ISR_PROTOS(fpu_not_avail)

	ISR_PROTOS_W_ECODE(double_fault)         

	ISR_PROTOS(cop_segment_overrun)

	ISR_PROTOS_W_ECODE(invalid_tss)          
	ISR_PROTOS_W_ECODE(segment_not_present)
	ISR_PROTOS_W_ECODE(stack_exception)
	ISR_PROTOS_W_ECODE(general_prot_fault)
	ISR_PROTOS_W_ECODE(page_fault)            

	ISR_PROTOS(floating_point_error)
	ISR_PROTOS(alignment_check)
	ISR_PROTOS(machine_check)

	ISR_PROTOS(IRQ_0)
	ISR_PROTOS(IRQ_1)
	ISR_PROTOS(IRQ_2)
	ISR_PROTOS(IRQ_3)
	ISR_PROTOS(IRQ_4)
	ISR_PROTOS(IRQ_5)
	ISR_PROTOS(IRQ_6)
	ISR_PROTOS(IRQ_7)
	ISR_PROTOS(IRQ_8)
	ISR_PROTOS(IRQ_9)
	ISR_PROTOS(IRQ_A)
	ISR_PROTOS(IRQ_B)
	ISR_PROTOS(IRQ_C)
	ISR_PROTOS(IRQ_D)
	ISR_PROTOS(IRQ_E)
	ISR_PROTOS(IRQ_F)

	ISR_PROTOS(SYS_CALL)

	void test1();
	void int_handler();
};

#define I(name) 	__ISR_ ## name ## _asm
#define ID		InterruptDescriptor 
#define INTR_GATE	INTR_TYPE_386_INTR_GATE

ID interrupt_table[256] = 
{
	// 0x00 - 0x1F: CPU Exceptions

	ID( I(divide_by_zero),      INTR_GATE 	), // 00
	ID( I(debug_exception),     INTR_GATE 	),
	ID( I(nmi),                 INTR_GATE 	),
	ID( I(breakpoint),          INTR_GATE	),
	ID( I(overflow),            INTR_GATE 	),
	ID( I(bound_exception),     INTR_GATE 	), 
	ID( I(invalid_opcode),      INTR_GATE 	),
	ID( I(fpu_not_avail),       INTR_GATE 	),
	ID( I(double_fault),        INTR_GATE 	), // 08
	ID( I(cop_segment_overrun), INTR_GATE 	),
	ID( I(invalid_tss),         INTR_GATE 	), 
	ID( I(segment_not_present), INTR_GATE 	),		
	ID( I(stack_exception),     INTR_GATE 	),
	ID( I(general_prot_fault),  INTR_GATE 	),
	ID( I(page_fault),          INTR_GATE 	),
	ID(				 	), 
	ID( I(floating_point_error),INTR_GATE	), // 10
	ID( I(alignment_check),     INTR_GATE 	),
	ID( I(machine_check),       INTR_GATE 	),
	ID(					),
	ID(					),
	ID(					),
	ID(					), 
	ID(					),
	ID(					), // 18
	ID(					), 
	ID(					), 
	ID(					),
	ID(					), 
	ID(					),
	ID(					), 
	ID(					), 

	// 0x20 - 0x2E: IRQs

	ID( I(IRQ_0),		    INTR_GATE 	), // 20
	ID( I(IRQ_1),		    INTR_GATE 	), 
	ID( I(IRQ_2),		    INTR_GATE 	),
	ID( I(IRQ_3),		    INTR_GATE 	),
	ID( I(IRQ_4),		    INTR_GATE 	),
	ID( I(IRQ_5),		    INTR_GATE 	),
	ID( I(IRQ_6),		    INTR_GATE 	),
	ID( I(IRQ_7),		    INTR_GATE 	),
	ID( I(IRQ_8),		    INTR_GATE 	), // 28
	ID( I(IRQ_9),		    INTR_GATE	),
	ID( I(IRQ_A),		    INTR_GATE 	),
	ID( I(IRQ_B),		    INTR_GATE 	),
	ID( I(IRQ_C),		    INTR_GATE 	),
	ID( I(IRQ_D),		    INTR_GATE 	),
	ID( I(IRQ_E),		    INTR_GATE 	),
	ID( I(IRQ_F),		    INTR_GATE 	),
	
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), // 30
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), 
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), // 40
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), 
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), // 50
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), 
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), // 60
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), 
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), // 70
	ID(), ID(), ID(), ID(), ID(), ID(), ID(), ID(), 

	ID( I(SYS_CALL),	    INTR_GATE, INTR_ACCESS_RING_3),
};

static struct _idtr {
    unsigned char limit_7_0;
    unsigned char limit_15_8;
    unsigned char base_7_0;
    unsigned char base_15_8;
    unsigned char base_23_16;
    unsigned char base_31_24;
} idtr;

void load_idt(InterruptDescriptor *_base, int num)
{
	int limit = num * sizeof(InterruptDescriptor) - 1;	
	unsigned long base = (unsigned long)_base;

	idtr.limit_7_0  =  limit       & 0xff;
	idtr.limit_15_8 = (limit >> 8) & 0xff;

	idtr.base_7_0   =  base        & 0xff;
	idtr.base_15_8  = (base >> 8)  & 0xff;
	idtr.base_23_16 = (base >> 16) & 0xff;
	idtr.base_31_24 = (base >> 24) & 0xff;

	/* load interrupt descriptor table to IDTR */
	__asm__ __volatile__ ("lidt (%0)" : : "r" ((void *) &idtr));
}

void init_idt() 
{

	load_idt(interrupt_table, 256);
}

// IRQ0 - Timer
C_ISR(IRQ_0) 
{
	static int i = 0;
	i ++;
	if (i % (TIMER_TICKS_PER_SECOND * 10) == 0) {
		extern Scheduler scheduler;
		unlock_irq(1);
		scheduler.inc_ticks();
		scheduler.schedule();
	}
	unlock_irq(1);
}

// IRQ1 - Keyboard
C_ISR(IRQ_1)
{
	int val = inb(0x60);

#if MAGIC_SYSRQ_KEY
	if (val == 225) {
		print_kernel_state(r);
	}
#endif

	printk("Keyboard interrupt: %d\n", val);
	enable_ints();	
	unlock_irq(2);
}

static inline void out_status(int code) 
{
	farpokel(0x10, (void*)0xC00B8000, (unsigned)code);
	while (1) {
		__asm__ __volatile__ ("hlt");
	}
}

C_ISR(divide_by_zero)  { 
	out_status(' Z D'); 
}

C_ISR(debug_exception) { out_status(' D E'); }
C_ISR(nmi) 	       { out_status(' M N'); }
C_ISR(breakpoint)      { out_status(' P B'); }
C_ISR(overflow)        { out_status(' F O'); }
C_ISR(bound_exception) { out_status(' E B'); }
C_ISR(invalid_opcode)  { out_status(' O I'); }
C_ISR(fpu_not_avail)   { out_status(' A N'); }

// There's no way to recover from double fault, so we
// just 
C_ISR_W_ECODE(double_fault) { 
	out_status(' F D'); 
	__asm__ __volatile__("hlt"); 
}

C_ISR(cop_segment_overrun)  { out_status(' O C'); }

C_ISR_W_ECODE(invalid_tss)  { out_status(' T I'); }
C_ISR_W_ECODE(segment_not_present) { out_status(' P N'); }
C_ISR_W_ECODE(stack_exception)     { out_status(' E S'); }
C_ISR_W_ECODE(general_prot_fault)  { out_status(' P G'); }
C_ISR_W_ECODE(page_fault)          { 
	unsigned long addr;

	asm("mov %%cr2, %0"
	: "=r"(addr) : );

	printk("Page fault when accessing physical address: 0x%08x\n", addr);
	out_status(' F P'); 
}

C_ISR(floating_point_error) { out_status(' P F'); }
C_ISR(alignment_check)      { out_status(' C A'); }
C_ISR(machine_check)        { out_status(' C M'); }

C_ISR(IRQ_2) { }
C_ISR(IRQ_3) { }
C_ISR(IRQ_4) { }
C_ISR(IRQ_5) { }
C_ISR(IRQ_6) { }
C_ISR(IRQ_7) { }
C_ISR(IRQ_8) { }
C_ISR(IRQ_9) { }
C_ISR(IRQ_A) { }
C_ISR(IRQ_B) { }
C_ISR(IRQ_C) { }
C_ISR(IRQ_D) { }
C_ISR(IRQ_E) { 
#ifdef DEBUG
	printk("ide0: interrupt\n");
#endif
	// read request from mbox/mqueue
	// inform request owner that we are done
	unlock_irq(14);
}
C_ISR(IRQ_F) { printk("ide1: interrupt\n"); unlock_irq(15); }

volatile int __critical_nest_depth = 0;
