/****************************************************
 * This is the asm linkage boot part for kernel when
 * using multiboot standard.
 */

#define STACK_SIZE      8192
#define BOOT_LOG        0xC0008000
#define BOOT_PHYS         0x008000
                      //0x40100000

#define LOG_TO_PHYS(x)    (x - BOOT_LOG + BOOT_PHYS)
#define ADJUST_TO_PHYS(x) addl $(BOOT_LOG - BOOT_PHYS), x


#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)	_ ## sym
#else
# define EXT_C(sym)	sym
#endif

	.text
	.globl	_start
	.globl  _END_OF_BOOTLOADER

_start:
        /* set access for domain 0 to not managed */
        mov r0, #0x3
        mcr p15, 0, r0, c3, c0, 0

        /* load page table start address */
        ldr r0, =(LOG_TO_PHYS(__page_directory))
        mcr p15, 0, r0, c2, c0, 0

        /* enable MMU. bit 0: MMU enable, bit 23: ARMv6 tables */
        mrc p15, 0, r0, c1, c0, 0

	ldr r1, =0x800001
        orr r0, r0, r1

        mcr p15, 0, r0, c1, c0, 0

	/* prepare entry. lr = 0, fp = 0, set up stack */
	mov lr, #0
	mov fp, #0
        ldr sp, =(stack + STACK_SIZE - 4)

	bl kmain

loop:
	b loop

	/* dump literals here */
	.ltorg

__INIT_STACK_END:
	.long stack + STACK_SIZE - 4

_END_OF_BOOTLOADER:

	.data
	.comm stack, STACK_SIZE, 4096

	.data
	.comm __page_directory, 16384, 16384

	.align 14
__page_directory:
	/* identity map first 3G. 1M section size */
	.equ   PDT_ENTRY, 0xC02
	.rept  3072
	.long  PDT_ENTRY
	.set   PDT_ENTRY, PDT_ENTRY + 0x100000
	.endr

	/* map first 1G to higher half. 1M section size */
	.equ   PDT_ENTRY, 0xC02
	.rept  1024
	.long  PDT_ENTRY
	.set   PDT_ENTRY, PDT_ENTRY + 0x100000
	.endr

	.globl  _boot_heap_bytes_consumed
_boot_heap_bytes_consumed: .long 0
