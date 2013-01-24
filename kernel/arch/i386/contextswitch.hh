void __inline__ doContextSwitch(uint32_t esp, uint32_t *saved_esp) {
     __asm__ __volatile__ (
                "call 1f\n\t"
                "jmp 2f\n"
                "1:\n\t"
                "pushal\n\t"
                "mov %%esp, (%1)\n\t"
                "mov %0, %%esp\n\t"
                "popal\n\t"
                "ret\n"
                "2:\n\t"
                : : "a"(esp), "b"(saved_esp));
}

