#include "paging"
#include "printk.h"

#define LOG_TO_PHYS(x)     ((void*)((unsigned long)(x) - 0xC0000000)) 
#define PAGE_SIZE 	   0x1000
#define ENTRIES_PER_TABLE  0x400
#define PAGE_TABLE_ENTRIES (6 * ENTRIES_PER_TABLE)

PageTableEntry     page_directory[PAGE_SIZE]
	__attribute__ ((aligned (PAGE_SIZE)));
PageTableEntry     page_tables[PAGE_TABLE_ENTRIES]
	__attribute__ ((aligned (PAGE_SIZE)));

void _set_cr3(void *base) 
{
    asm("movl %0, %%cr3;"
	"jmp 0f;"
        "0: nop"
	: 
	: "r"(base));
}

struct _gdt_pos {
    unsigned short limit __attribute__((packed));
    unsigned long  base  __attribute__((packed));
} gdt_pos;

void _enable_paging_and_gdt(void *base, int limit) {
    gdt_pos.limit = limit;
    gdt_pos.base = (unsigned long)base;

    asm(
"	 hlt;movl %%cr0, %%eax;		"
"	 orl  $0x80000000, %%eax;	"
"	 movl %%eax, %%cr0;		"
"	 jmp 1f;			"
"1:;					"
"        lgdt gdt_pos;			"
"	 jmp 2f;			"
"2:;					"
"        mov $0x10, %%ax;		"	
"        mov %%ax, %%es;		"	
"	 ljmp $0x8, $new_cs;		"
"new_cs:;				"
"	 mov %%ax, %%ds;		"
"	 mov %%ax, %%es;		"
"	 mov %%ax, %%fs;		"
"	 mov %%ax, %%gs;		"
"	 mov %%ax, %%ss;		"
	: 
	: 
	: "%eax"); 
        /* segment registers actually haven't changed... */
}

void get_gdt_location(void* & location, int& limit);
void initialize_page_tables() 
{
	PageFlags flags;
	void *gdt_location;
	int gdt_limit;
	
	get_gdt_location(gdt_location, gdt_limit);

	flags.set_present(true);
	flags.set_rw(true);

	for (int i = 0; i < PAGE_TABLE_ENTRIES / ENTRIES_PER_TABLE; i ++) {
		page_directory[i].set_flags(flags);
		page_directory[i].set_offset(
			LOG_TO_PHYS(page_tables + i * ENTRIES_PER_TABLE)
		);
		page_directory[i + 0x300].set_flags(flags);
		page_directory[i + 0x300].set_offset(
			LOG_TO_PHYS(page_tables + i * ENTRIES_PER_TABLE)
		);
	}

	char *base_address = 0;
	for (int i = 0; i < PAGE_TABLE_ENTRIES; i ++) {
		page_tables[i].set_flags(flags);
		page_tables[i].set_offset(base_address);
		base_address += PAGE_SIZE;
	}

	_set_cr3(LOG_TO_PHYS(page_directory));
	_enable_paging_and_gdt(gdt_location, gdt_limit);
}
