/****************************************************
 * This is the asm linkage boot part for kernel when
 * using multiboot standard.
 */

#define MB_HEADER_MAGIC 0x1BADB002
#define MB_HEADER_FLAGS 0x00000003
#define MB_INFO_MAGIC   0x2BADB002
#define STACK_SIZE      16384
#define __BOOT_CS       0x8

#define BOOT_LOG        0xC0100000
#define BOOT_PHYS         0x100000
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

	.align	4
_multiboot_header:
	.long	MB_HEADER_MAGIC
	.long	MB_HEADER_FLAGS
	.long	-(MB_HEADER_MAGIC + MB_HEADER_FLAGS)
	/* use ELF format for kernel? */
_start:
	/* save arguments! */
	movl    %eax, %edx
	cmpl    $MB_INFO_MAGIC, %edx
	je      _store_multiboot_info

_multiboot_info_stored:	

/**************************/
/* INITIALIZE PAGE TABLE! */
/**************************/
	mov     $LOG_TO_PHYS(__page_directory), %eax
	mov	%eax, %cr3
        
        mov     %cr4, %ecx
        or      $0x00000010, %ecx  
        mov     %ecx, %cr4

	mov	%cr0, %eax
	or	$0x80000000, %eax // Set PG bit
	mov	%eax, %cr0	  // Paging is on!
	jmp	_ptflush	  // Flush the instruction queue.

_ptflush:

/*************************************************/
/* PAGE TABLE INITIALIZED: 0xC0000000 WORKS NOW! */
/*************************************************/
        lgdtl   gdt_48			    # load gdt with whatever is
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
	/* push parameters, ebx = pointer to mb info, edx = magick number */
	pushl	%ebx
	pushl	%edx

	call	EXT_C(kmain)
	/* print a halt message */
	pushl	$halt_message
	call	EXT_C(printk)

_halt:
	cli
	hlt
	jmp	_halt













#define COPY_FIELD(x)			\
	movl	LOG_TO_PHYS(x), %esi;   \
	movl	%edi, LOG_TO_PHYS(x);	\
	ADJUST_TO_PHYS(x)		


_store_multiboot_info:
	cld

	movl	$(multiboot_record_length >> 2), %ecx
	movl	%ebx, %esi
	movl    $LOG_TO_PHYS(multiboot_header), %edi
	rep	movsd
	
	/* get flags in edx */
	movl    LOG_TO_PHYS(mb_flags), %edx

	/* get heap pointer */
	movl	$LOG_TO_PHYS(_BOOT_HEAP_START), %edi

	test	$4, %edx
	jz	no_cmdline

	COPY_FIELD(mb_cmdline)


cmdline_loop:
	lodsb
	stosb
	testb   %al, %al
	jnz     cmdline_loop

	
no_cmdline:
	/* mods available? */
	test    $8, %edx
	jz      no_modules

	movl	LOG_TO_PHYS(mb_mods_count), %ecx
	test    %ecx, %ecx
	jz	no_modules

	/* multiply count by 4 (entry size is 16... dwords in move) */
	shll	$2, %ecx

	COPY_FIELD(mb_mods_addr)
	rep	movsl

no_modules:
	/* mmap available? */
	test    $64, %edx
	jz      no_mmap

	movl	LOG_TO_PHYS(mb_mmap_length), %ecx
	test    %ecx, %ecx
	jz	no_mmap

	/* divide by 4 (dwords in move) */
	shrl	$2, %ecx

	COPY_FIELD(mb_mods_addr)
	rep	movsl

no_mmap:

	/* restore magic in edx */
	movl	$MB_INFO_MAGIC,	%edx

	subl	$LOG_TO_PHYS(_BOOT_HEAP_START), %edi
	movl	%edi, LOG_TO_PHYS(_boot_heap_bytes_consumed)
	
	/* actual virtual address! */
	movl    $multiboot_header, %ebx

	jmp     _multiboot_info_stored


_END_OF_BOOTLOADER:

	.data

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
        .word   gdt_end - gdt - 1               /* gdt limit */
        .long   gdt		                /* gdt base (filled in later) */

	.align 4096
	/* boot.S stack */
	.comm stack, STACK_SIZE

	.data
        .align 4096


	.globl __page_directory


__page_directory:
	/* identity map first 3G. 4M pagesize */
	.equ   PDT_ENTRY, 0x83
	.rept  768
	.long  PDT_ENTRY
	.set   PDT_ENTRY, PDT_ENTRY + 0x400000
	.endr

	/* map first 1G to higher half. 4M pagesize */
	.equ   PDT_ENTRY, 0x83
	.rept  256
	.long  PDT_ENTRY
	.set   PDT_ENTRY, PDT_ENTRY + 0x400000
	.endr

multiboot_header:
mb_flags:              .long 0
mb_mem_lower:          .long 0
mb_mem_upper:          .long 0
mb_boot_device:	       .long 0
mb_cmdline:            .long 0
mb_mods_count:         .long 0
mb_mods_addr:	       .long 0
mb_syms:               .long 0
		       .long 0
		       .long 0
                       .long 0
mb_mmap_length:	       .long 0
mb_mmap_addr:	       .long 0
mb_drives_length:      .long 0
mb_drives_addr:        .long 0
mb_config_table:       .long 0
mb_boot_loader_name:   .long 0
mb_apm_table:          .long 0
mb_vbe_control_info:   .long 0
mb_vbe_mode_info:      .long 0
mb_vbe_mode:           .short 0
mb_vbe_interface_seg:  .short 0
mb_vbe_interface_off:  .short 0
mb_vbe_interface_len:  .short 0
	.set multiboot_record_length, . - multiboot_header

	.globl  _boot_heap_bytes_consumed
_boot_heap_bytes_consumed: .long 0
