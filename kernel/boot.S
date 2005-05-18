/****************************************************
 * This is the asm linkage boot part for kernel when
 * using multiboot standard.
 */

#define MB_HEADER_MAGIC 0x1BADB002
#define MB_HEADER_FLAGS 0x00000003
#define STACK_SIZE      16384
#define __BOOT_CS       0x8

#define BOOT_LOG        0xC0100000
#define BOOT_PHYS         0x100000
                      //0x40100000

#define LOG_TO_PHYS(x) (x - BOOT_LOG + BOOT_PHYS)

#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)	_ ## sym
#else
# define EXT_C(sym)	sym
#endif

	.text
	.globl	_start

_start:
	jmp	_multiboot_start
	.align	4
	
_multiboot_header:
	.long	MB_HEADER_MAGIC
	.long	MB_HEADER_FLAGS
	.long	-(MB_HEADER_MAGIC + MB_HEADER_FLAGS)
	/* use ELF format for kernel? */
_multiboot_start:
        lgdtl   LOG_TO_PHYS(gdt_48)             # load gdt with whatever is
                                                # appropriate
	movl    %eax, %ecx
	movl    $0x10, %eax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    %ax, %ss

#	movl 	$0x41414141, %eax
#	movl    %eax, 0xC00B8000
#	hlt
	.byte   0xea                        # prefix + jmpi-opcode
	.long   start_code                  # will be set to 0x100000
                                            # for big kernels
        .word   __BOOT_CS
start_code:
	/* init stackpointer */
	movl	$(stack + STACK_SIZE), %esp
	/* reset EFLAGS */
	pushl	$0
	popf
	/* push parameters, ebx = pointer to mb info, eax = magick number */
	pushl	%ebx
	pushl	%ecx

	call	EXT_C(kmain)
	/* print a halt message */
	pushl	$halt_message
	call	EXT_C(printk)
_halt:
	hlt
	jmp	_halt

halt_message:
	.asciz	"Halted.\n"

        .align 32
gdt:
	.word   0
	.word   0
	.word   0
	.word   0

        .word   0xFFFF                          # 4Gb - (0x100000*0x1000 = 4Gb)
        .word   0x0000                          # base address = 0
        .word   0x9A00                          # code read/exec
        .word   0x40CF                          # granularity = 4096, 386
                                                #  (+5th nibble of limit)

        .word   0xFFFF                          # 4Gb - (0x100000*0x1000 = 4Gb)
        .word   0x0000                          # base address = 0
        .word   0x9200                          # data read/write
        .word   0x40CF                          # granularity = 4096, 386
                                                #  (+5th nibble of limit)
gdt_end:

	.align 16
	.word   0
gdt_48:
        .word   gdt_end - gdt - 1               # gdt limit
        .long   LOG_TO_PHYS(gdt)                # gdt base (filled in later)

	
	/* boot.S stack */
	.comm stack, STACK_SIZE
