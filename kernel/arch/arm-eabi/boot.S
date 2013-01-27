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
        /* make page table */
        ldr r3, =(LOG_TO_PHYS(__page_directory))
        /* move the base address to temp register */
        mov r4, r3
        /* value for first */
        ldr r5, =(0xC02)
        /* increment */
        ldr r6, =(0x100000)
        /* num iterations */
        ldr r7, =(3072)
_map_0_3G:
        /* store the entry */
        stmia r4!, {r5}
        /* add page increment */
        add r5, r5, r6
        /* decrement the counter */
        subs r7, r7, #1
	/* and loop if not zero */
	bne _map_0_3G

	ldr r5, =(0xC02)
	ldr r7, =(1024)

_map_3_4G:
        /* store the entry */
        stmia r4!, {r5}
        /* add page increment */
        add r5, r5, r6
        /* decrement the counter */
        subs r7, r7, #1
	/* and loop if not zero */
	bne _map_3_4G

        /* set access for domain 0 to not managed */
        mov r4, #0x3
        mcr p15, 0, r4, c3, c0, 0

        /* load page table start address, still in r3 */
        mcr p15, 0, r3, c2, c0, 0

        /* enable MMU. bit 0: MMU enable, bit 23: ARMv6 tables */
        mrc p15, 0, r3, c1, c0, 0

	ldr r4, =0x800001
        orr r3, r3, r4

        mcr p15, 0, r3, c1, c0, 0

	/* prepare entry. lr = 0, fp = 0, set up stack */
	mov lr, #0
	mov fp, #0
        ldr sp, =(stack + STACK_SIZE - 4)

	bl kmain

loop:
	b loop

	/* dump literals here */
	.ltorg

_END_OF_BOOTLOADER:

	.data
	.comm stack, STACK_SIZE, 4096

	.data
	.comm __page_directory, 16384, 16384

	.globl  _boot_heap_bytes_consumed
_boot_heap_bytes_consumed: .long 0
