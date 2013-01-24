// -*- C++ -*-

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <inttypes.h>

union Registers {
    struct {
        uint32_t r0,  r1,  r2,  r3;
	uint32_t r4,  r5,  r6,  r7;
	uint32_t r8,  r9,  r10, r11;
        uint32_t r12, r13, r14, r15;
    };
    struct {
        uint32_t arg0, arg1, arg2, arg3;
        uint32_t arg4, arg5, __dummy;
        union {
            uint32_t syscallNumber;
            uint32_t syscallRetval;
        };
    };
};

static __inline__ bool interruptsEnabled() {
        register uint32_t tmp;
        asm volatile ("msr %0, spsr" :: "r" (tmp));
        return !(bool)(tmp & 0x80);
}

static __inline__ bool disableInterrupts() {
        register uint32_t oldf;
        register uint32_t tmp;
        asm volatile ("mrs %[oldf], cpsr\n\t"
                      "orr %[tmp], %[oldf], #0x80\n\t"
                      "msr cpsr_c, %[tmp]"
                     : [oldf] "=r" (oldf), [tmp] "=r" (tmp)
                     :
                     : "memory", "cc");

        return !(bool)(oldf & 0x80);
}

static __inline__ void __enableInterrupts() {
        register uint32_t tmp;
        asm volatile ("mrs %[tmp], cpsr\n\t"
                      "bic %[tmp], %[tmp], #0x80\n\t"
                      "msr cpsr_c, %[tmp]"
                      : [tmp] "=r" (tmp) : : "memory", "cc");
}

static __inline__ void enableInterruptsIf(register bool enable) {
	if (enable) {
                __enableInterrupts();
	}
}

#endif
