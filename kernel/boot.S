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

#define PRESENT_AND_RW  3
#define PAGE_DIR_ENTRY  \
	(LOG_TO_PHYS(__page_tables) + PRESENT_AND_RW)

#define LOG_TO_PHYS(x)  (x - BOOT_LOG + BOOT_PHYS)

#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)	_ ## sym
#else
# define EXT_C(sym)	sym
#endif

	.text
	.globl	_start
	.globl  _END_OF_BOOTLOADER
	.align	4
_multiboot_header:
	.long	MB_HEADER_MAGIC
	.long	MB_HEADER_FLAGS
	.long	-(MB_HEADER_MAGIC + MB_HEADER_FLAGS)
	/* use ELF format for kernel? */
_start:
	/* save arguments! */
	movl    %eax, %edx

	movl    $PAGE_DIR_ENTRY, %eax
	movl    %eax, LOG_TO_PHYS(__page_directory)
	movl    %eax, LOG_TO_PHYS(__page_directory) + 0xC00

	movl    $0x400, %ecx
	movl    $3, %eax
	movl 	$LOG_TO_PHYS(__page_tables), %edi
	cld

_loop:
	stosl
	addl    $0x1000, %eax
	loop _loop

/**************************/
/* INITIALIZE PAGE TABLE! */
/**************************/
	mov     $LOG_TO_PHYS(__page_directory), %eax
	mov	%eax, %cr3

	mov	%cr0, %eax
	or	$0x80000000, %eax // Set PG bit
	mov	%eax, %cr0	  // Paging is on!
	jmp	_ptflush	  // Flush the instruction queue.

_ptflush:

/*************************************************/
/* PAGE TABLE INITIALIZED: 0xC0000000 WORKS NOW! */
/*************************************************/
        lgdtl   gdt_48                      # load gdt with whatever is
                                            # appropriate
	movl    $0x10, %eax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	movw    %ax, %ss

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
	/* push parameters, edi = pointer to mb info, esi = magick number */
	pushl	%ebx
	pushl	%edx

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
        .word   0x00CF                          # granularity = 4096, 386
                                                #  (+5th nibble of base)

        .word   0xFFFF                          # 4Gb - (0x100000*0x1000 = 4Gb)
        .word   0x0000                          # base address = 0
        .word   0x9200                          # data read/write
        .word   0x00CF                          # granularity = 4096, 386
                                                #  (+5th nibble of base)
gdt_end:

	.align 16
	.word   0
gdt_48:
        .word   gdt_end - gdt - 1               # gdt limit
        .long   gdt		                # gdt base (filled in later)

	.align 4096
	/* boot.S stack */
	.comm stack, STACK_SIZE

	.align 4096
	.comm __page_directory, 4096, 4096
	.align 4096
	.comm __page_tables,    4096, 4096

_END_OF_BOOTLOADER:
