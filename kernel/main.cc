#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>

#include "kernel/mm/kmalloc.h"
#include "kernel/arch/current/cpuid.hh"
#include "kernel/atomic.hh"
#include "kernel/refcount.hh"
#include "kernel/mm/memarea.hh"
#include "kernel/ktasks/softirq.hh"
#include "kernel/drivers/ramdisk/ramdiskdevice.hh"
#include "kernel/drivers/harddisk/harddiskdevice.hh"
#include "kernel/fs/zsosrdfs/zsosrdfs.hh"
#include "kernel/fs/path.hh"
#include "kernel/fs/harddisk.hh"
#include "kernel/fs/fatfs/fat.hh"
#include "kernel/arch/current/halt.hh"
#include "kernel/arch/current/fpu.hh"
#include "kernel/arch/current/stacktrace.hh"
#include "kernel/arch/current/keyboard.hh"


#include "kernel/syscall.hh"
#include "kernel/printk.h"
#include "multiboot.hh"
#include "interrupt.hh"
#include "paging.hh"
#include "usertask.hh"
#include "kerneltask.hh"
#include "init_vga.hh"


#include "kernel/arch/current/simpleide.hh"
#include "scheduler.hh"
#include "timer.hh"
#include "panic.hh"

#ifndef __arm__
#define HAS_PCI
#include "arch/current/pci.hh"
#include "arch/current/ide.hh"

#else
void initPic() {
}

#endif

#include "mutex.hh"
#include "init.hh"
#include "hexdump.hh"

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
		kout << "\t\tmodule " << (i + 1) << " args: " << multiboot_info->get_module(i).get_string() << endl;

		char *data = (char *)multiboot_info->get_module(i).get_base();
		kout << "\t\t\tbase address: " << (void*)data << endl;

		printk("\t\t");
		for (int j = 0; j < 16; j++) {
			printk("%c", data[j]);
		}
		printk("\n");
	}

	kout << "--  --  --" << endl;
}

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

void initializePageFrameTable(MultibootInfo& boot_info, Allocator& allocator);

RingBuffer<int> *buf;
void startIdleTask();

extern void __set_default_allocator(Allocator *new_def);

void timerRoutine(int vector) {
     // kout << "TIMER ROUTINE..." << endl;
}

ZsosRdFs       root_filesystem;
RamDiskDevice *root_blk_dev;

void mountRoot() {
        int nmods = multiboot_info->number_of_modules();
	if (nmods < 1) {
		kernelPanic("No modules loaded - no initrd");
	}

        char *data = (char *)multiboot_info->get_module(0).get_base();
	uint32_t len = multiboot_info->get_module(0).get_length();

	root_blk_dev = new RamDiskDevice(data + 0xC0000000, len);
	ErrnoCode rc = root_filesystem.initialize(*root_blk_dev);
	if (rc != NOERROR) {
		kout << "Nonzero status while mounting root file system " << rc << endl;
		kernelPanic("The root file system is invalid!");
	}
}

#include "kernel/exe/bits.hh"

void ok() {
	kout << "\033[65G\033[0;1m[\033[1;32mOK\033[0;1m]\033[m" << endl;
}

void epic_fail() {
	kout << "\033[65G\033[0;1m[\033[1;32mEPIC FAIL\033[0;1m]\033[m" << endl;
}

char databuf[513];
HardDisk        *rootHardDisk;
HardDiskDevice  *rootHdDevice;
Partition       *rootPartition;
FatFileSystem   *rootFileSystem;


void initializeHardDisk() {
	kout << "Initializing HD...";
	reset_ide_disk();
	// todo fillin actual parms
	rootHdDevice = new HardDiskDevice(55000000);
	rootHardDisk = new HardDisk(*rootHdDevice);
	rootHardDisk->initialize();
	int numpart = rootHardDisk->getNumPartitions();
	kout << "Root Hard Disk - " << numpart << " partitions";
	ok();

	if (numpart == 0) {
		kernelPanic("No partitions on hard disk??");
	}

	rootPartition = rootHardDisk->getPartition(0);
	rootFileSystem = new FatFileSystem(*rootPartition);
}

void kmallocTest() {
    void *memory = kmalloc(1024);
    //void *memory2 = kmalloc(4096);
    kfree(memory);
    //kfree(memory2);
}

extern "C" void kernel_main(unsigned int magic, void *mbd)
{
	kout << "Detecting CPU:";
	detectCpu();

	if (! cpuIdentity.getFlags() & CpuIdentity::FLAG_PSE) {
		kernelPanic("The CPU does not support 4 MiB pages!");
	}
	ok();

	kout << "Setting boot time dynamic memory allocator";
	__set_default_allocator(&boot_dynmem_alloc);
	ok();

	kout << "Setting up descriptor tables: ";
	init_gdt();
	kout << "GDT";
	init_idt();
	kout << ", IDT";
	ok();

	kout << "Multiboot information:" << endl;
	extract_multiboot_info(magic, mbd);

	kout << "Initializing MM: ";
	initializePageFrameTable(*multiboot_info, boot_dynmem_alloc);
	kout << frames.getLastPageNumber() << " pages of RAM.";
	ok();

	kout << "Building kernel page table";
	initialize_page_tables();
	kout << ", page tables";
//	disable_null_page();
//	kout << ", null page";
	ok();

	kout << "Initializing kmalloc";
	kmallocInit();
	ok();

	kout << "Testing kmalloc... ";
	kmallocTest();
	ok();

	kout << "Initializing PIC";
	initPic();
	ok();

        kout << "Initializing FPU";
        initialize_FPU();
        ok();

	kout << "Initializing tasking";
	initialize_tasking();
	ok();

	kout << "Initializing timer";
	initialize_timer();
	ok();

#ifdef HAS_PCI
	kout << "Detecting PCI devices" << endl;
	PCI::initialize();
	ok();
#endif

        kout << "Enabling SoftIRQs";
	initSoftIrq();
	registerSoftIrq(1, timerRoutine);
	ok();

	kout << "Starting idle task";
	startIdleTask();
	ok();

	kout << "Initializing keyboard";
	if (! initKeyboard()) {
		epic_fail();
                kernelPanic("KB init failed");
        }
	ok();

	kout << "Running miscellaneous initialization hooks";
//	Init::run(Init::EARLY);
//	Init::run(Init::LATE);
	ok();

/*	kout << "Testing IDE commands";
	char testdata[512];
	ide::controller.ifs[0]->add_request(0, 0, 0, 1L, 1);
	ok(); */

	kout << "Mounting root filesystem";
	mountRoot();
	ok();

	kout << "Initializing hard disk";
	initializeHardDisk();

	kout << "Preparing init process, pid 1";
//	kernelPanic("Fell out from scheduling loop!\n");
//	halt();

//        kout << "Are we runnable" << _binary_ld_linux_so_2_start.isRunnable();
//       ok();

//	tesmi.initialize(&_binary_example_zsx_start);
	tesmi.initialize(&_binary_ld_linux_so_2_start);
	tesmi.setProcessId(1);
	scheduler.addTask(&tesmi);
	ok();

	kout << "Kernel initialization complete. Entering init.\n";

	scheduler.schedule();
	halt();
	kernelPanic("Fell out from scheduling loop!\n");
}
