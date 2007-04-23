#include <iostream>
#include <cstring>

#include "task.hh"
#include "scheduler.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

#include "tss.hh"

#include "kernel/exe/bits.hh"
#include "kernel/paging.hh"

#include "kernel/mm/memarea.hh"
#include "kernel/mm/mm.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/mm/zeropageloader.hh"

#include "kernel/panic.hh"

TssContents tssSegment __attribute__((aligned(4096)));

UserTask::UserTask(const char *name, State state, int priority) 
	: Task(name, state, priority) 
{

}

void UserTask::enable_io() {
	// set IOPL to 3...
	unsigned int *tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	tmp -= 3;	
	*tmp |= 3 << 12;
}

extern "C" void ____user_task_dispatch_new_asm(); 

void UserTask::initialize(ZsosExeHeader *hdr) {
	unsigned int *tmp;

	header = hdr;
	
	// build kernel stack
	tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack)) - 1;
	kstack = (unsigned int)tmp;
	tmp --;

	*tmp -- = USER_DATA_DESCRIPTOR + 3;
	*tmp -- = 0xC0000000UL - 4;
        *tmp -- = 0x0202;

        *tmp -- = USER_CODE_DESCRIPTOR + 3;
        *tmp -- = (unsigned int)hdr->entryPoint; 

        *tmp -- = USER_DATA_DESCRIPTOR + 3; // ds
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // es
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // fs
        *tmp -- = USER_DATA_DESCRIPTOR + 3; // gs

	// return here after switching stacks...
	*tmp -- = (uint32_t)____user_task_dispatch_new_asm;

        *tmp -- = 0;    // ebp
        *tmp -- = 0;    // esp
        *tmp -- = 0;    // edi
        *tmp -- = 0;    // esi
        *tmp -- = 0;    // edx
        *tmp -- = 0;    // ecx
        *tmp -- = 0;    // ebx
        *tmp    = 0;    // eax

	esp = (unsigned int)tmp;

	memmap = new MemMap();
	MemMapArea *m;

	m = new MemMapArea(
		VirtAddr((void*)hdr->textVirt), 
		VirtAddr((void*)(hdr->textVirt + hdr->textLength)));
	int rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	m = new MemMapArea(
		VirtAddr((void*)hdr->dataVirt), 
		VirtAddr((void*)(hdr->dataVirt + hdr->dataLength)));
	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	m = new MemMapArea(
		VirtAddr((void*)hdr->bssVirt), 
		VirtAddr((void*)(hdr->bssVirt + hdr->bssLength)));
	m->setLoader(&zeroPageLoader);
	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	m = new MemMapArea(
		VirtAddr((void*)(0xC0000000 - 0x100000)), 
		VirtAddr((void*)(0xC0000000)));
	m->setLoader(&zeroPageLoader);
	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	memmap->dumpMemMap();
	return;		
error:
	kernelPanic("Error in UserTask::initialize");
}

UserTask::~UserTask() {
}

void ____user_task_dispatch_new() {
	__asm__ __volatile__(
		".globl ____user_task_dispatch_new_asm\n"
		"____user_task_dispatch_new_asm:\n\t"
		"popl %%gs\n\t"
        	"popl %%fs\n\t"
        	"popl %%es\n\t"
        	"popl %%ds\n\t"
	        "iret\n\t"
	: : );
}

void UserTask::dispatch(uint32_t *saved_esp) {
	tssSegment.esp0 = kstack;
	Task::switchContexts(saved_esp);
}

void UserTask::terminate() {
	extern Scheduler scheduler;
	setCurrentState(TERMINATED);
	scheduler.removeTask(this);
	scheduler.schedule();
}

bool UserTask::handlePageFault(PageFaultInfo& f) {
        kout << "Process " << process_id << " had a pfault at " 
		<< (uint32_t)f.address << endl;

	MemMapArea *m = memmap->findAreaByAddr(f.address);
	if (! m) {
		kernelPanic("User task killed...\n");
	}

	else {
		PageLoader *p = m->getLoader();
		if (p) {
			p->loadPage(this, 
				m,
				f.write ? MemMapArea::W : MemMapArea::R, 
				f.address);
			kout << "Mapped...\n";
			return true;
		}
	}

	// map code...
	if (f.address == 1048740) {
		char *text_address = (char*)&_binary_example_zsx_start 
			+ _binary_example_zsx_start.textPhys;
		
		pageaddr_t p = pageaddr_t::fromVirtual(text_address);
	
		PageFlags fl(PageFlags::PRESENT | PageFlags::USER);
		mapPage(page_directory, f.address, 
			&(page_frames.getByFrame(p)), fl);
	}

	// map data (* should use COW *)...
	else if ((f.address & 0xF0000000) == 0x80000000) {
		PageFrame *frm;

		char *data_address = (char*)&_binary_example_zsx_start 
			+ _binary_example_zsx_start.dataPhys;
			
		pageaddr_t p = pageaddr_t::fromVirtual(data_address);
		frm = &(page_frames.getByFrame(p));

		PageFlags fl(PageFlags::PRESENT | PageFlags::USER | PageFlags::READWRITE);

		mapPage(page_directory, f.address, frm, fl);
	}
	else {
        	__asm__ __volatile__ ("cli; hlt");
	}
        return true;
}


void TssContents::setup() {
	memset(&tssSegment, 0, sizeof(tssSegment));
	ss0   = KERNEL_DATA_DESCRIPTOR;

        es = 
	ss = 
	ds = 
	fs = 
	gs = USER_DATA_DESCRIPTOR + 3;

        cs = USER_CODE_DESCRIPTOR + 3;

	eflags = 2;
	bitmap = 104;

	__asm__ __volatile__ ("mov %%cr3, %0" : "=a"(cr3) : );
}

void initialize_tasking() {
	tssSegment.setup();

	uint16_t tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr %0" : : "r"(tss_desc));
}

