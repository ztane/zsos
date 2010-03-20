#ifndef __FPU_HH_DEFINED__
#define __FPU_HH_DEFINED__

#include "controlregs.hh"

static void clear_TS_in_CR0() {
    clear_CR0_bits(CR0_BIT_TS);
}

static void set_TS_in_CR0() {
    set_CR0_bits(CR0_BIT_TS);
}

static void initialize_FPU() {
    clear_TS_in_CR0();
    set_CR0_bits(CR0_BIT_MP);
    clear_CR0_bits(CR0_BIT_EM);
    
    __asm__ __volatile__("fninit");
}

static void FPU_save_with_FNSAVE(uint8_t *data) {
    __asm__ __volatile__("fnsave %0\n\tfwait\n\t":"=m" (*data));
}

static void FPU_restore_with_FRSTOR(uint8_t *data) {
    __asm__ __volatile__("frstor %0":"=m" (*data));
}

#endif
