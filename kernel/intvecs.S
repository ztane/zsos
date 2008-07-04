#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)	_ ## sym
#else
# define EXT_C(sym)	sym
#endif

#define ASMISR(name) __ISR_ ## name ## _asm
#define ISR(name)    __ISR_ ## name 

// return w/ skip error code ;)
#define POP_IRETE			 	 \
        popal                                   ;\
        pop %gs                                 ;\
        pop %fs                                 ;\
        pop %es                                 ;\
        pop %ds                                 ;\
	add $4, %esp;				;\
        iret                              	;\
      
#define POP_IRET			 	 \
        popal                                   ;\
        pop %gs                                 ;\
        pop %fs                                 ;\
        pop %es                                 ;\
        pop %ds                                 ;\
        iret                              	;\
      

#define NEEDS_SCHEDULING			      \
	testl $0xFFFFFFFF, (__needs_scheduling_flag) ;


#define DEFINE_INTR_GATE(name)			 \
	.globl ASMISR(name)			;\
ASMISR(name):					;\
	push %ds				;\
	push %es				;\
	push %fs				;\
	push %gs				;\
	pushal					;\
	call ISR(name)				;\
						;\
	NEEDS_SCHEDULING			;\
	jne   1f		          	;\
0:		                          	;\
	POP_IRET				;\
1:						;\
	call __EXECUTE_SCHEDULER	  	;\
	POP_IRET				;


#define DEFINE_INTRE_GATE(name)			 \
	.globl ASMISR(name)			;\
ASMISR(name):					;\
	push %ds				;\
	push %es				;\
	push %fs				;\
	push %gs				;\
	pushal					;\
	call ISR(name)				;\
						;\
	NEEDS_SCHEDULING			;\
	jne   1f		          	;\
0:		                          	;\
	POP_IRETE				;\
1:						;\
	call __EXECUTE_SCHEDULER	  	;\
	POP_IRETE				


/*********************************************************
 * CODE BEGINS                                           *
 *********************************************************/

	.text

#define INTR_DIVIDE_BY_ZERO      0
#define INTR_DEBUG_EXCEPTION     1
#define INTR_NMI                 2
#define INTR_BREAKPOINT          3
#define INTR_OVERFLOW            4
#define INTR_BOUND_EXCEPTION     5
#define INTR_INVALID_OPCODE      6
#define INTR_FPU_NOT_AVAIL   	 7
#define INTR_DOUBLE_FAULT    	 8
#define INTR_COP_SEGMENT_OVERRUN 9
#define INTR_INVALID_TSS     	 10
#define INTR_SEGMENT_NOT_PRESENT 11
#define INTR_STACK_EXCEPTION 	 12
#define INTR_GENERAL_PROT_FAULT  13
#define INTR_PAGE_FAULT      	 14
#define INTR_FLP_ERROR       	 16
#define INTR_ALIGN_CHECK     	 17
#define INTR_MACHINE_CHECK   	 18

DEFINE_INTR_GATE(divide_by_zero)
DEFINE_INTR_GATE(debug_exception)
DEFINE_INTR_GATE(nmi)
DEFINE_INTR_GATE(breakpoint)
DEFINE_INTR_GATE(overflow)
DEFINE_INTR_GATE(bound_exception)
DEFINE_INTR_GATE(invalid_opcode)
DEFINE_INTR_GATE(fpu_not_avail)
DEFINE_INTRE_GATE(double_fault)		// w/ error
DEFINE_INTR_GATE(cop_segment_overrun)	
DEFINE_INTRE_GATE(invalid_tss)		// w/ error
DEFINE_INTRE_GATE(segment_not_present)   // w/ error
DEFINE_INTRE_GATE(stack_exception)       // w/ error
DEFINE_INTRE_GATE(general_prot_fault)    // w/ error
DEFINE_INTRE_GATE(page_fault)            // w/ error
DEFINE_INTR_GATE(floating_point_error)
DEFINE_INTR_GATE(alignment_check)
DEFINE_INTR_GATE(machine_check)

DEFINE_INTR_GATE(IRQ_0)
DEFINE_INTR_GATE(IRQ_1)
DEFINE_INTR_GATE(IRQ_2)
DEFINE_INTR_GATE(IRQ_3)
DEFINE_INTR_GATE(IRQ_4)
DEFINE_INTR_GATE(IRQ_5)
DEFINE_INTR_GATE(IRQ_6)
DEFINE_INTR_GATE(IRQ_7)
DEFINE_INTR_GATE(IRQ_8)
DEFINE_INTR_GATE(IRQ_9)
DEFINE_INTR_GATE(IRQ_A)
DEFINE_INTR_GATE(IRQ_B)
DEFINE_INTR_GATE(IRQ_C)
DEFINE_INTR_GATE(IRQ_D)
DEFINE_INTR_GATE(IRQ_E)
DEFINE_INTR_GATE(IRQ_F)

// DEFINE_INTR_GATE(SYS_CALL)

#include "syscall_table.S"
#define N_SYSCALLS 		(SYSCALL_TABLE_SIZE >> 2)

	.globl __ISR_SYS_CALL_asm

__ISR_SYS_CALL_asm:
        push %ds                
        push %es                
        push %fs                
        push %gs                
        pushal
	
	cmpl  $(N_SYSCALLS), %eax
        jae  __BAD_SYSCALL
	call *SYSCALL_VECTORS(, %eax, 4)

__RET_FROM_SYSCALL:
	NEEDS_SCHEDULING
	jnz __CALL_EXECUTE_SCHEDULER
	POP_IRET

__BAD_SYSCALL:
	call sys_BAD
	jmp  __RET_FROM_SYSCALL

__CALL_EXECUTE_SCHEDULER:
	call __EXECUTE_SCHEDULER
	POP_IRET



// does the job of executing bottom halves!
__EXECUTE_SCHEDULER:
	call	__execute_scheduler
	// release lock now! do atomic check on bh flags instead!
	NEEDS_SCHEDULING
	jnz __EXECUTE_SCHEDULER
	ret
