#include "paging"
#include "printk.h"

#define LOG_TO_PHYS(x)     ((void*)((unsigned long)(x) - 0xC0000000)) 
#define PAGE_SIZE 	   0x1000
#define ENTRIES_PER_TABLE  0x400
#define PAGE_TABLE_ENTRIES (ENTRIES_PER_TABLE)

extern char __page_directory[];
extern char __page_tables[];
extern char _END_OF_KERNEL;
extern char _END_OF_BOOTLOADER;

// no initializers!
PageTableEntry *page_directory = reinterpret_cast<PageTableEntry *>(__page_directory);
PageTableEntry *page_tables    = reinterpret_cast<PageTableEntry *>(__page_tables);

void _set_cr3(void *base) 
{
    asm("movl %0, %%cr3;"
	"jmp 0f;"
        "0: nop"
	: 
	: "r"(base));
}

void _test_segments() 
{
    asm("mov $10, %%ax;"
	"mov %%ax, %%es;" 
	: : : "%eax");
}

void _reenable_paging() {
    asm(
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
		(unsigned long)LOG_TO_PHYS(&_END_OF_KERNEL) },
	{ 0,   0        },
};

void initialize_page_tables() 
{
	PageFlags flags;
	
	flags.set_flags(4);
	flags.set_present(true);
	flags.set_rw(true);

	page_directory[0x300].set_flags(flags);
	page_directory[0x300].set_offset(
		LOG_TO_PHYS(page_tables)
	);
	page_directory[0x0].set_flags(flags);
	page_directory[0x0].set_offset(
		LOG_TO_PHYS(page_tables)
	);

	int idx = 0;
	while (initially_mapped[idx].base != initially_mapped[idx].top) 
	{
		unsigned long base = initially_mapped[idx].base;
		unsigned long top  = initially_mapped[idx].top;
		
		printk("\tMapping memory %08X - %08X\n", base, top);
		while (base < top) {
			page_tables[base / 0x1000].set_flags(flags);
			page_tables[base / 0x1000].set_offset(base);
			base += 0x1000;
		}
		idx ++;
	}
 
	_set_cr3(LOG_TO_PHYS(page_directory));
	_reenable_paging();
}


