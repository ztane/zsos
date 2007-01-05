#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <kernel/cpuid.hh>
#include <kernel/atomic.hh>
#include <kernel/refcount.hh>
#include <kernel/mm/memarea.hh>

#include "syscall.hh"
#include "printk.h"
#include "multiboot.hh"
#include "interrupt.hh"
#include "paging.hh"
#include "tasking.hh"
#include "init_vga.hh"
#include "ide.hh"
#include "scheduler.hh"
#include "timer.hh"
#include <panic.hh>
#include <pci.hh>
#include "mutex.hh"
#include <new>
#include "init.hh"

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

void user_task2() {
	for (int j = 0; j < 100; j++)
	for (int i = 'A'; i < 'Z'; i++)
		sem_post(i);

	while(1);
}

void user_task() {
	while (1) {
		//write_character(sem_wait());
		sem_wait();
	}
}

#include "ringbuffer.hh"

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

void initializePageFrameTable(const MultibootInfo& boot_info, Allocator& allocator);

RingBuffer<int> *buf;

extern "C" void kernel_main(unsigned int magic, void *mbd);
void kernel_main(unsigned int magic, void *mbd)
{
//	kout << "Detecting CPU:" << endl;
//	detect_cpu();

//	if (! cpu_identity.get_flags() & CPUIdentity::FLAG_PSE) 
//	{
//		kernel_panic("The CPU does not support 4 MiB pages!");
//	}

//	kout << "Setting up GDT...";
	init_gdt();
//	kout << " done..." << endl;

//	kout << "Setting up IDT...";
	init_idt();
//	kout << " done" << endl;

//	kout << "Remapping PIC...";
	init_pic();
//	kout << " done" << endl;
	
//	kout << "Enabling initial paging..." << endl;
	initialize_page_tables();
//	kout << "Paging enabled." << endl;

//	kout << "Multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);

//	kout << "Disabling boot-time double paging...";
	disable_null_page();
//	kout << "done." << endl; 

//	kout << "Initializing page frame structures: " << endl;
	initializePageFrameTable(*multiboot_info, boot_dynmem_alloc);
//	kout << global_page_frame_table.getLastPage();
//	kout << " pages of RAM." << endl;

//	kout << "Initializing tasking...";
	initialize_tasking();
//	kout << " done" << endl;

//	kout << "Initializing timer...";
	initialize_timer();
//	kout << " done" << endl;

//	kout << "Enabling keyboard and timer interrupts...";
	enable_keyboard();
//	kout << " done" << endl;

	extern void __set_default_allocator(Allocator *new_def);
	__set_default_allocator(&boot_dynmem_alloc);

//	kout << "Detecting PCI devices..." << endl;
//	PCI::initialize();

	buf = new RingBuffer<int>(10);

	IDE::controller.dummy();

	kout << "Testing init..." << endl;
	init::run();

	kout << "Starting tasking...";
	tesmi.initialize((void*)user_task);
	tesmi.setProcessId(1);
	tesmi2.initialize((void*)user_task2);
	tesmi2.setProcessId(2);
	scheduler.add_process(&tesmi);
	scheduler.add_process(&tesmi2);
	scheduler.schedule();

	kernel_panic("Fell out from scheduling loop!\n");
}

