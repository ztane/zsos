#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>

#include "kernel/cpuid.hh"
#include "kernel/atomic.hh"
#include "kernel/refcount.hh"
#include "kernel/mm/memarea.hh"
#include "kernel/ktasks/softirq.hh"

#include "kernel/syscall.hh"
#include "kernel/printk.h"
#include "multiboot.hh"
#include "interrupt.hh"
#include "paging.hh"
#include "usertask.hh"
#include "kerneltask.hh"
#include "init_vga.hh"
#include "ide.hh"
#include "scheduler.hh"
#include "timer.hh"
#include "panic.hh"
#include "pci.hh"
#include "mutex.hh"
#include "init.hh"

#include "kernel/fs/filedescriptor.hh"

extern FileLike *getConsoleDriver();

void print_memmap(MultibootInfo *mbi) 
{
	int ct = mbi->number_of_mmap_entries();
	
	for (int i = 0; i < ct; i++) {
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
//		haltloop();
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

	int nmods = multiboot_info->number_of_modules();

	kout << "\tnumber of modules: " << nmods << endl;

	for (int i = 0; i < nmods; i ++) {
		kout << "\t\tmodule" << i << " args: " << multiboot_info->get_module(i).get_string() << endl;

		char *data = (char *)multiboot_info->get_module(i).get_base();

		printk("\t\t");
		for (int j = 0; j < 16; j++) {
			printk("%c", data[j]);
		}
		printk("\n");
	}

	kout << "--  --  --" << endl;
	kernelPanic("stop");
}

extern void enable_keyboard();
extern bool initKeyboard();
extern void init_idt();
extern void init_gdt();
extern void initialize_tasking();

#include "ringbuffer.hh"

void detectCpu() {
	cpuIdentity.identify();
	
	kout << "\tCPU0:" << endl;
	kout << "\t-----" << endl;
	kout << "\tVendor:\t\t" << cpuIdentity.getVendor()   << endl;
	kout << "\tName:\t\t"   << cpuIdentity.getProcessorName()   << endl;
	kout << "\tFamily:\t\t" << cpuIdentity.getFamily()   << endl;
	kout << "\tModel:\t\t"  << cpuIdentity.getModel()    << endl;
	kout << "\tStepping:\t" << cpuIdentity.getStepping() << endl;
	kout << "\tFeatures:\t";
	
	uint32_t flags = cpuIdentity.getFlags();
	int flag_no = 0;
	int pretty_ct = 0;

	while (flags != 0) {
		if (flags & 1) {
			kout << CpuIdentity::getFlagName(flag_no) << "\t";
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

UserTask tesmi("tesmi");
Scheduler scheduler;

void initializePageFrameTable(const MultibootInfo& boot_info, Allocator& allocator);

RingBuffer<int> *buf;
void startIdleTask();

extern void __set_default_allocator(Allocator *new_def);

void timerRoutine(int vector) {
     // kout << "TIMER ROUTINE..." << endl;
}

#include "kernel/exe/bits.hh"

extern "C" void kernel_main(unsigned int magic, void *mbd)
{
	kout << "Detecting CPU:" << endl;
	detectCpu();

	if (! cpuIdentity.getFlags() & CpuIdentity::FLAG_PSE) {
		kernelPanic("The CPU does not support 4 MiB pages!");
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

	kout << "Multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);

	kout << "Initializing page frame structures: " << endl;
	initializePageFrameTable(*multiboot_info, boot_dynmem_alloc);
	kout << page_frames.getLastPage();
	kout << " pages of RAM." << endl;
	
	kout << "Enabling initial paging..." << endl;

	initialize_page_tables();
	kout << "Paging enabled." << endl;

	kout << "Disabling boot-time double paging...";
	disable_null_page();
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

	__set_default_allocator(&boot_dynmem_alloc);

	kout << "Detecting PCI devices..." << endl;
	PCI::initialize();

	buf = new RingBuffer<int>(10);

	Init::run(Init::EARLY);
	Init::run(Init::LATE);
	
	kout << "Starting tasking..." << endl;

	initSoftIrq();
	registerSoftIrq(1, timerRoutine);
        kout << " SoftIRQ, ";
	
	startIdleTask();
        kout << " idle task done" << endl;

	kout << "Initializing keyboard...";
	if (! initKeyboard()) {
		kout << " FAILED!" << endl;
                kernelPanic("KB init failed");
        }
        kout << " done" << endl;

	kout << "Testing IDE command" << endl;
	char testdata[512];
	ide::controller.ifs[0]->add_request(0, 0, 0, 0L, 1);

	kout << "Starting init...";
	tesmi.initialize(&_binary_example_zsx_start);
	tesmi.setProcessId(1);
	scheduler.addTask(&tesmi);

	kout << " done\nNow entering init:\n";
	scheduler.schedule();
	kernelPanic("Fell out from scheduling loop!\n");
}

