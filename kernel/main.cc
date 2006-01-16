#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "printk.h"
#include "multiboot.h"
#include "interrupt.h"
#include "paging"
#include "tasking"

void *PHYS_TO_LOG(void *addr) 
{
	return (void*)((unsigned int)addr);
}

void print_memmap(MultibootInfo *mbi) 
{
	unsigned int mmap_addr   = mbi->get_mmap_addr(),
		     mmap_length = mbi->get_mmap_length();
	multiboot_mmap_t *mmap = (multiboot_mmap_t *)(mmap_addr);
	
        kout << "\tmmap provided by loader:" << endl;
	
        for (; (unsigned int)(mmap) < mmap_addr + mmap_length;
               mmap = (multiboot_mmap_t *)((unsigned int)(mmap) + mmap->size + sizeof(mmap->size)))
        {
		printk("\tbase: 0x%08x - length: 0x%08x, type: 0x%08x\n",
             //        	mmap->base_addr_high,
                      	mmap->base_addr_low,
             //       	mmap->length_high,
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
	printk("\tmagic: 0x%x, mbd: 0x%x\n", magic, mbd);

	if (!mbi)
		return;
	
	/* NOTE! if MultibootInfo::get_[low|high]_mem() returns
	 * 0 it means that the boot loader didn't provide the
	 * amount of memory available or it really is 0 :).
	 */
	if (mbi->get_flags() & MB_FLAG_MEM)
		kout << "\tlower memory: "    << mbi->get_low_mem()
		     << "kiB, upper memory: " << mbi->get_high_mem()
		     << "kiB" << endl;

	if (mbi->get_flags() & MB_FLAG_LOADER_NAME)
		kout << "\tloader: " << mbi->get_loader_name() << endl;

	if (mbi->get_flags() & MB_FLAG_CMDLINE)
		kout << "\tcmdline: " << mbi->get_cmdline() << endl;

	if (mbi->get_flags() & MB_FLAG_MMAP)
		print_memmap(mbi);
}

extern void enable_keyboard();
extern void init_idt();
extern void init_gdt();
extern void initialize_tasking();

void user_task() {
	while (1) {
		__asm__ __volatile__(
			"mov $0x42, %eax\n\t"
			"int $0x80\n\t"
		);
	}
}

void user_task2() {
	while (1) {
		__asm__ __volatile__(
			"mov $0x41, %eax\n\t"
			"int $0x80\n\t"
		);
	}
}

void haltloop() 
{
	while(1) {
		__asm__ __volatile__("hlt");
	}
}

Process tesmi("tesmi");
Process tesmi2("tesmi2");

extern "C" void kernel_main(unsigned int magic, void *mbd);
void kernel_main(unsigned int magic, void *mbd)
{
	kout << "Initializing..." << endl;
	kout << "Setting up GDT...";
	init_gdt();
	kout << " done..." << endl;

	kout << "Setting up IDT...";
	init_idt();
	kout << " done" << endl;

	kout << "Remapping PIC...";
	init_pic();
	kout << " done" << endl;
	
	kout << "Enabling paging...";
	initialize_page_tables();
	kout << " done" << endl;

	kout << "Recovering multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);
	kout << "-- -- --" << endl;
	
	kout << "Enabling keyboard and timer interrupts...";
	enable_keyboard();
	kout << " done" << endl;

	kout << "Initializing tasking...";
	initialize_tasking();
	kout << " done" << endl;
	
	kout << "Starting tasking...";
	tesmi.initialize((void*)user_task);
	tesmi2.initialize((void*)user_task2);
	tesmi.dispatch();

	haltloop();

	return;
}
