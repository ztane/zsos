	.globl ARM_INTVEC_TABLE

ARM_INTVEC_TABLE:
        ldr pc, RESET_HANDLER
        ldr pc, UNDEFINED_HANDLER
        ldr pc, SWI_HANDLER
        ldr pc, PREFETCH_ABORT_HANDLER
        ldr pc, ABORT_HANDLER
        nop
        ldr pc, IRQ_HANDLER

FIQ_HANDLER:
        subs pc, r14_fiq, #4

RESET_HANDLER:
        nop
	b RESET_HANDLER

UNDEFINED_HANDLER:
        movs pc, r14_und

SWI_HANDLER:
	movs pc, r14_svc

PREFETCH_ABORT_HANDLER:
	subs pc, r14_abt, #4

ABORT_HANDLER:
	subs pc, r14_abt, #8

IRQ_HANDLER:
        subs pc, r14_irq, #4

HANDLER_RETURN:
        ldmfd sp!,{R0-R12,pc}^
