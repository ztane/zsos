OUTPUT_FORMAT("binary")
ENTRY(__CRT_startup)

virt = 0x100000;
phys = 0x0;

SECTIONS {
     .text virt : AT(phys) {
       __SECTION_CODE = .;            /* symbols to mark start of section */
       *(.text*)                      /* wildcard for "gcc -ffunction-sections" */
       *(.gnu.linkonce.t.*)           /* C++ templates and non-inlined inlines */
       *(.rodata*)                    /* read-only data */
       *(.gnu.linkonce.r.*)
       . = ALIGN(4096);
     }

     .data : AT(phys + (__SECTION_DATA - __SECTION_CODE)) {
       __SECTION_DATA = .;
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

     .bss : AT(phys + (__SECTION_BSS - __SECTION_CODE)) {
       __SECTION_BSS = .;
       *(.bss*)                       /* wildcard for "gcc -fdata-sections" */
       *(.gnu.linkonce.b.*)
       *(COMMON)                      /* "common" variables */
       . = ALIGN(4096);
     }
     __END_OF_EXECUTABLE = .;
}
