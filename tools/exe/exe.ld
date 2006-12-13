OUTPUT_FORMAT("binary")
ENTRY(__CRT_startup)

virt     =   0x100000;
virtdata = 0x80000000;
phys     = 0x0;

hdrpos   = phys;
hdrlen   = __HEADER_END - __HEADER;

textpos  = phys + hdrlen;
textlen  = __SECTION_TEXT_END - __SECTION_TEXT;

datapos  = phys + hdrlen + textlen;
datalen  = __SECTION_DATA_END - __SECTION_DATA;

bsspos   = phys + hdrlen + textlen + datalen;
bsslen   = __SECTION_BSS_END - __SECTION_BSS;

SECTIONS {
     .header : AT(phys) {
        __HEADER = .;
	/* Magic: "ZSOSEXE" */
	LONG(0x534F535A);
	LONG(0x00455845);

	LONG(hdrpos);
	LONG(hdrlen);

	LONG(textpos);
	LONG(__SECTION_TEXT);
	LONG(textlen);
	
	LONG(datapos);
	LONG(__SECTION_DATA);
	LONG(datalen);

	LONG(bsspos);
	LONG(__SECTION_BSS);
	LONG(bsslen);

	. = ALIGN(4096);
	__HEADER_END = .;
     }

     . = virt;
     .text : AT(phys + hdrlen) {
       __SECTION_TEXT = .;            /* symbols to mark start of section */
       *(.text*)                      /* wildcard for "gcc -ffunction-sections" */
       *(.gnu.linkonce.t.*)           /* C++ templates and non-inlined inlines */
       *(.rodata*)                    /* read-only data */
       *(.gnu.linkonce.r.*)
       . = ALIGN(4096);
       __SECTION_TEXT_END = .;
     }

     . = virtdata;
     .data : AT(phys + hdrlen + textlen) {
       __SECTION_DATA = .;
       *(.data*)                      /* wildcard for "gcc -fdata-sections" */
       *(.gnu.linkonce.d.*)

       /* contructors */
       __CTOR_LIST__ = .;
       LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
       *(SORT_BY_NAME(.ctors))
       LONG(0)
       __CTOR_END__ = .;

       __DTOR_LIST__ = .;
       LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
       *(SORT_BY_NAME(.dtors))
       LONG(0)
       __DTOR_END__ = .;

       . = ALIGN(4096);
       __SECTION_DATA_END = .;
     }

     .bss : AT(phys + hdrlen + textlen + datalen) {
       __SECTION_BSS = .;
       *(.bss*)                       /* wildcard for "gcc -fdata-sections" */
       *(.gnu.linkonce.b.*)
       *(COMMON)                      /* "common" variables */
       . = ALIGN(4096);
	__SECTION_BSS_END = .;
     }

     __END_OF_EXECUTABLE = .;
}
