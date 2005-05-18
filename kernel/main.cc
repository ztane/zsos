#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "printk.h"
#include "multiboot.h"
#include "interrupt.h"
#include "paging"

void *PHYS_TO_LOG(void *addr) 
{
	return (void*)((unsigned int)addr + 0xC0000000);
}

void print_memmap(MultibootInfo *mbi) 
{
	unsigned int mmap_addr   = mbi->get_mmap_addr(),
		     mmap_length = mbi->get_mmap_length();
	multiboot_mmap_t *mmap = (multiboot_mmap_t *)(mmap_addr);
	
        kout << "mmap provided by loader:" << endl;
	
        for (; (unsigned int)(mmap) < mmap_addr + mmap_length;
               mmap = (multiboot_mmap_t *)((unsigned int)(mmap) + mmap->size + sizeof(mmap->size)))
        {
		printk("base: 0x%x%x - length: 0x%x%x, type: 0x%x\n",
                       	mmap->base_addr_high,
                      	mmap->base_addr_low,
                       	mmap->length_high,
                       	mmap->length_low,
                       	mmap->type);
        }
}

void extract_multiboot_info(unsigned int magic, void *mbd) 
{
	/* black magic? no! */
	MultibootInfo *mbi = magic == MB_MAGIC_VALUE ? 
		static_cast<MultibootInfo *>(PHYS_TO_LOG(mbd)) : NULL;
	
	/* there's no hex printing in kout yet */
	printk("magic: 0x%x, mbd: 0x%x\n", magic, mbd);

	if (!mbi)
		return;
	
	/* NOTE! if MultibootInfo::get_[low|high]_mem() returns
	 * 0 it means that the boot loader didn't provide the
	 * amount of memory available or it really is 0 :).
	 */
	if (mbi->get_flags() & MB_FLAG_MEM)
		kout << "lower mem: "    << mbi->get_low_mem()
		     << "k, upper mem: " << mbi->get_high_mem()
		     << "k" << endl;

	if (mbi->get_flags() & MB_FLAG_LOADER_NAME)
		kout << "loader: " << mbi->get_loader_name() << endl;

	if (mbi->get_flags() & MB_FLAG_CMDLINE)
		kout << "cmdline: " << mbi->get_cmdline() << endl;

	if (mbi->get_flags() & MB_FLAG_MMAP)
		print_memmap(mbi);
}

extern void enable_keyboard();
extern void init_idt();


void haltloop() 
{
	while(1) {
		__asm__ __volatile__("hlt");
	}
}

extern "C" void kernel_main(unsigned int magic, void *mbd);
void kernel_main(unsigned int magic, void *mbd)
{
	kout << "Initializing..." << endl;
	kout << "Remapping PIC...";
	init_pic();
	kout << " done" << endl;

	kout << "Setting up null IDT...";
	init_idt();
	kout << " done" << endl;
	
	kout << "Recovering multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);
	kout << " done" << endl;
	
	kout << "Enabling paging...";
	initialize_page_tables();
	kout << " done" << endl;

	kout << "Enabling keyboard and timer interrupts...";
	enable_keyboard();
	kout << " done" << endl;

	kout << "Enabling interrupts...";
	enable_ints();
	kout << " done" << endl;
	
	*((char*)0x10000000) = 0;

	kout << "Entering halt loop." << endl;
	haltloop();

	return;
}
