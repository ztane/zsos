#include "paging"
#include "printk.h"

#define LOG_TO_PHYS(x)     ((void*)((unsigned long)(x) - 0xC0000000)) 
#define PAGE_SIZE 	   0x1000
#define ENTRIES_PER_TABLE  0x400
#define PAGE_TABLE_ENTRIES (ENTRIES_PER_TABLE)

extern char __page_directory[];
extern char __page_tables[];
extern char _END_OF_BOOTLOADER;
extern char _BOOT_HEAP_END;

// no initializers!
PageTableEntry *page_directory = reinterpret_cast<PageTableEntry *>(__page_directory);
PageTableEntry *page_tables    = reinterpret_cast<PageTableEntry *>(__page_tables);

static void _set_cr3(void *base) 
{
    __asm__ __volatile__(
	"movl %0, %%cr3;"
	"jmp 0f;"
        "0: nop"
	: 
	: "r"(base));
}

static void _enable_pse()
{
	__asm__ __volatile__ (
		"movl %%cr4, %%eax;"
		"orl $0x10, %%eax;"
		"movl %%eax, %%cr4;"
		: : :"%eax"
	);
}

static void _reenable_paging() {
	__asm__ __volatile__ (
"	 movl %%cr0, %%eax;		"
"	 orl  $0x80000000, %%eax;	"
"	 movl %%eax, %%cr0;		"
"	 jmp 1f;			"
"1:;					"
	: 
	: 
	: "%eax"); 
        /* segment registers actually haven't changed... */
}

struct MemoryArea {
	unsigned long base;
	unsigned long top;
};

MemoryArea initially_mapped[] = {
	{ 0x0, 0x100000 },
	{ (unsigned long)LOG_TO_PHYS(&_END_OF_BOOTLOADER), 
		(unsigned long)LOG_TO_PHYS(&_BOOT_HEAP_END) },
	{ 0,   0        },
};

void initialize_page_tables() 
{
	PageFlags flags;
	
	_enable_pse();
	
	flags.set_flags(4);
	flags.set_present(true);
	flags.set_rw(true);
	flags.set_4MiB(true);

	// temporarily map to 0 as well.
	page_directory[0x0].set_flags(flags);
	page_directory[0x0].set_offset((void*)0);
 
	// map fisrt 1 GiB of physmem starting from 3 GiB,
	// using 4 MiB pages
	char *ptr = 0;
	for (int i = 0x300; i < 0x400; i ++, ptr += 0x400000) {
		page_directory[i].set_flags(flags);
		page_directory[i].set_offset(ptr);
	}

	_set_cr3(LOG_TO_PHYS(page_directory));
	_reenable_paging();
}

void disable_null_page() 
{
	page_directory[0x0].clear();
	_reenable_paging();
}
