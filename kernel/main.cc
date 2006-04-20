#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <kernel/cpuid>

#include "syscall"
#include "printk.h"
#include "multiboot"
#include "interrupt"
#include "paging"
#include "tasking"
#include "init_vga"
#include "ide"
#include "scheduler"
#include "timer"
#include "mm/freepagelist"
#include <panic>

void haltloop() 
{
	while(1) {
		__asm__ __volatile__("hlt");
	}
}

void print_memmap(MultibootInfo *mbi) 
{
	int ct = mbi->number_of_mmap_entries();
	
	for (int i = 0; i < ct; i++) 
	{
		const MultibootMMapInfo& inf =
			mbi->mmap_entry(i);
		
		
		printk("\tbase: 0x%08x - length: 0x%08x\n",
                      	inf.get_base(),
                       	inf.get_length());
	}
}

MultibootInfo *multiboot_info = NULL;

void extract_multiboot_info(unsigned int magic, void *mbd) 
{
	/* there's no hex printing in kout yet */
	printk("\tmagic: 0x%x, mbd: 0x%x\n", magic, mbd);

	if (MB_MAGIC_VALUE != magic || mbd == 0) {
		kout << "ERROR - NO MULTIBOOT INFO PRESENT!!!" << endl;
		haltloop();
	}

	multiboot_info = new (boot_dynmem_alloc) MultibootInfo
		(mbd, boot_dynmem_alloc);

	/* NOTE! if MultibootInfo::get_[low|high]_mem() returns
	 * 0 it means that the boot loader didn't provide the
	 * amount of memory available or it really is 0 :).
	 */
	if (multiboot_info->get_flags() & MB_FLAG_MEM)
		kout << "\tlower memory: "    << multiboot_info->get_low_mem()
		     << "kiB, upper memory: " << multiboot_info->get_high_mem()
		     << "kiB" << endl;

	if (multiboot_info->get_flags() & MB_FLAG_LOADER_NAME)
		kout << "\tloader: " << multiboot_info->get_loader_name() << endl;

	if (multiboot_info->get_flags() & MB_FLAG_CMDLINE)
		kout << "\tcmdline: " << multiboot_info->get_cmdline() << endl;

	if (multiboot_info->get_flags() & MB_FLAG_MMAP)
		print_memmap(multiboot_info);
	
	printk("\tMax usable ram address: 0x%08x\n", 
		multiboot_info->get_max_ram_address());

	kout << "--  --  --" << endl;
}

extern void enable_keyboard();
extern void init_idt();
extern void init_gdt();
extern void initialize_tasking();

void user_task() {
	while (1) {
		become_io_task();
		kout << get_process_id() << endl;
		printk("abc\n");
		write_character('A');
	}
}

void user_task2() {
	while (1) {
		write_character('B');
	}
}

void detect_cpu() {
	cpu_identity.identify();
	
	kout << "\tCPU0:" << endl;
	kout << "\t-----" << endl;
	kout << "\tVendor:\t\t" << cpu_identity.get_vendor()   << endl;
	kout << "\tName:\t\t" << cpu_identity.get_processor_name()   << endl;
	kout << "\tFamily:\t\t" << cpu_identity.get_family()   << endl;
	kout << "\tModel:\t\t"  << cpu_identity.get_model()    << endl;
	kout << "\tStepping:\t" << cpu_identity.get_stepping() << endl;
	kout << "\tFeatures:\t";
	
	uint32_t flags = cpu_identity.get_flags();
	int flag_no = 0;
	int pretty_ct = 0;

	while (flags != 0) {
		if (flags & 1) {
			kout << CPUIdentity::get_flag_name(flag_no) << "\t";
			pretty_ct ++;
			if ((pretty_ct & 3) == 0) {
				kout << endl << "\t\t\t";
			}
		}

		flag_no ++;
		flags >>= 1;
	}

	kout << endl;
}

Process tesmi("tesmi");
Process tesmi2("tesmi2");
Scheduler scheduler;

extern "C" void kernel_main(unsigned int magic, void *mbd);
void kernel_main(unsigned int magic, void *mbd)
{
	kout << "Detecting CPU:" << endl;
	detect_cpu();

	if (! cpu_identity.get_flags() & CPUIdentity::FLAG_PSE) 
	{
		kernel_panic("The CPU does not support 4 MiB pages!");
	}

	kout << "Setting up GDT...";
	init_gdt();
	kout << " done..." << endl;

	kout << "Setting up IDT...";
	init_idt();
	kout << " done" << endl;

	kout << "Remapping PIC...";
	init_pic();
	kout << " done" << endl;
	
	kout << "Enabling initial paging..." << endl;
	initialize_page_tables();
	kout << "Paging enabled." << endl;

	kout << "Multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);

	kout << "Initializing free page tables...";
	free_page_list.initialize(*multiboot_info, boot_dynmem_alloc);
	kout << "done." << endl;

	kout << "Initializing tasking...";
	initialize_tasking();
	kout << " done" << endl;

	kout << "Initializing timer...";
	initialize_timer();
	kout << " done" << endl;

	kout << "Enabling keyboard and timer interrupts...";
	enable_keyboard();
	kout << " done" << endl;

	haltloop();

	kout << "Starting tasking...";
	tesmi.initialize((void*)user_task);
	tesmi2.initialize((void*)user_task2);
	scheduler.add_process(&tesmi);
	scheduler.add_process(&tesmi2);
	scheduler.schedule();

	kernel_panic("Fell out from scheduling loop!\n");
}
