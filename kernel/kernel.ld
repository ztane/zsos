OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

virt = 0xC0100000;
phys =   0x100000;

SECTIONS {
     .text virt : AT(phys) {
       g_code = .;                    /* symbols to mark start of section */
       *(.text*)                      /* wildcard for "gcc -ffunction-sections" */
       *(.gnu.linkonce.t.*)           /* C++ templates and non-inlined inlines */
       *(.rodata*)                    /* read-only data */
       *(.gnu.linkonce.r.*)
       . = ALIGN(4096);
     }

     .data : AT(phys + (g_data - g_code)) {
       g_data = .;
       *(.data*)                      /* wildcard for "gcc -fdata-sections" */
       *(.gnu.linkonce.d.*)

       /* contructors */
       __CTOR_LIST__ = .;
       LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
       *(SORT(.ctor*))
       LONG(0)
       __CTOR_END__ = .;

       . = ALIGN(4096);
     }

     .bss : AT(phys + (g_bss - g_code)) {
       g_bss = .;
       *(.bss*)                       /* wildcard for "gcc -fdata-sections" */
       *(.gnu.linkonce.b.*)
       *(COMMON)                      /* "common" variables */
     }
     g_end = .;
     _END_OF_KERNEL = .;
     . = ALIGN(4096);
     _BOOT_HEAP_START = .;
     . = . + 262144;
     _BOOT_HEAP_END   = .;
}
