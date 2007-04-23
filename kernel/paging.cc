#include "kernel/paging.hh"
#include "kernel/printk.h"
#include "kernel/panic.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/mm/memarea.hh"
#include "kernel/interrupt.hh"

#define LOG_TO_PHYS(x)     ((void*)((unsigned long)(x) - 0xC0000000)) 
#define PAGE_SIZE 	   0x1000
#define ENTRIES_PER_TABLE  0x400
#define PAGE_TABLE_ENTRIES (ENTRIES_PER_TABLE)

extern char __page_directory[];
extern char __page_tables[];
extern char _END_OF_BOOTLOADER;
extern char _BOOT_HEAP_END;

// no initializers!
PageDirectory  *page_directory = reinterpret_cast<PageDirectory *>(__page_directory);

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

struct __MemoryArea {
	unsigned long base;
	unsigned long top;
};

__MemoryArea initially_mapped[] = {
	{ 0x0, 0x100000 },
	{ (unsigned long)LOG_TO_PHYS(&_END_OF_BOOTLOADER), 
		(unsigned long)LOG_TO_PHYS(&_BOOT_HEAP_END) },
	{ 0,   0        },
};

pageaddr_t getMappedPage(PageDirectory *d, VirtAddr addr) {
	PageTableEntry *e = d->getPTE(addr);

	if (e && e->getFlags().isPresent()) {
		return e->getPageAddr();
	}

	return 0xFFFFFFFF;
}

uint32_t mapPage(PageDirectory *d, VirtAddr a, PageFrame *f, PageFlags fl) {
	PageTableEntry *e = d->getPTE(a);

	bool irqs = disableInterrupts();
	if (! e) {
		PageAllocation ac;
		NormalMemory.allocatePages(1, ac);
		if (! ac) {
			kernelPanic("Could not allocate PTE\n");
		}

		PageTable *ptr = (PageTable *)ac.getAddress().toVirtual();
		ptr->clear();
		
		PageDirectoryEntry tmp;
		PageFlags fl;
		fl.setPresent(true);
		fl.setRW(true);
		fl.setUser(true);

		tmp.setFlags(fl);
		tmp.setVirtAddr(ptr);

		*(d->getPDE(a)) = tmp;
		e = d->getPTE(a);
	}
	
	PageTableEntry tmp;
	tmp.setPageAddr(f->getPageAddr());
	tmp.setFlags(fl);
	*e = tmp;

	_set_cr3(LOG_TO_PHYS(d));
	enableInterruptsIf(irqs);
	return 0;
}

void initialize_page_tables() 
{
	PageFlags flags;
	
	_enable_pse();
	
	flags.setFlags(0);
	flags.setPresent(true);
	flags.setRW(false);
	flags.setHuge(true);

	// temporarily map to 0 as well.
	page_directory->getPDE(VirtAddr(0U))->setFlags(flags);
	page_directory->getPDE(VirtAddr(0U))->setPageAddr(0);
 
	// map fisrt 1 GiB of physmem starting from 3 GiB,
	// using 4 MiB pages
	int ptr = 0;
	for (uint32_t i = 0xC0000000UL; i < 0xF0000000UL; 
		i += 0x400000, ptr += 0x400) 
	{
		PageDirectoryEntry *e = page_directory->getPDE(VirtAddr((void*)i));
		e->setFlags(flags);
		e->setPageAddr(ptr);
	}

	for (uint32_t i = 0x00000000UL; i < 0xC0000000UL; i += 0x400000)
	{
		page_directory->getPDE(VirtAddr((void*)i))->clear();
	}

	_set_cr3(LOG_TO_PHYS(page_directory));
	_reenable_paging();
}

void disable_null_page() 
{
	page_directory->getPDE(VirtAddr(0U))->clear();
	_set_cr3(LOG_TO_PHYS(page_directory));
}
