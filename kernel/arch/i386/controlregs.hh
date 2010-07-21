#ifndef CONTROLREGS_HH_INCLUDED
#define CONTROLREGS_HH_INCLUDED

static const int CR0_BIT_TS = 1 << 3;
static const int CR0_BIT_EM = 1 << 2;
static const int CR0_BIT_MP = 1 << 1;

static void set_CR0_bits(uint32_t bits) {
        __asm__ __volatile__ (
                "movl %%cr0, %%eax;"
                "orl %%edx, %%eax;"
                "movl %%eax, %%cr0;"
                : : "d"(bits) :"%eax"
        );
}

static void clear_CR0_bits(uint32_t bits) {
        bits = ~bits;
        __asm__ __volatile__ (
                "movl %%cr0, %%eax;"
                "andl %%edx, %%eax;"
                "movl %%eax, %%cr0;"
                : : "d"(bits) :"%eax"
        );
}

#endif
