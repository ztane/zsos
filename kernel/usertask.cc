#include <iostream>
#include <cstring>

#include "task.hh"
#include "scheduler.hh"
#include "usertask.hh"
#include "memory.hh"
#include "printk.h"

#include "tss.hh"

#include "kernel/arch/current/fpu.hh"
#include "kernel/arch/current/reboot.hh"
#include "kernel/exe/bits.hh"
#include "kernel/paging.hh"

#include "kernel/mm/memarea.hh"
#include "kernel/mm/mm.hh"
#include "kernel/mm/pageframe.hh"
#include "kernel/mm/zeropageloader.hh"
#include "kernel/mm/textpageloader.hh"
#include "kernel/mm/datapageloader.hh"

#include "kernel/panic.hh"
#include "kernel/consoledriver.hh"
#include "kernel/printstate.hh"


TssContents tssSegment __attribute__((aligned(4096)));

UserTask::UserTask(const char *name, State state, int priority)
	: Task(name, state, priority)
{
	for (int i = 0; i < MAX_FILEDES; i++) {
		fileDescriptors[i] = NULL;
	}
}

void UserTask::enable_io() {
	// set IOPL to 3...
	unsigned int *tmp = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	tmp -= 3;
	*tmp |= 3 << 12;
}

extern "C" void ____user_task_dispatch_new_asm();

static void initialize_stdstreams(UserTask *task) {
	// create stdin
        FileDescriptor *fd;
        ErrnoCode errno = getConsoleDriver()->open(FileDescriptor::READ, fd);
	if (errno != NOERROR) {
		kernelPanic("Failed to open terminal for reading!");
	}
	task->fileDescriptors[0] = fd;

	// create stdout
        errno = getConsoleDriver()->open(FileDescriptor::WRITE, fd);
	if (errno != NOERROR) {
		kernelPanic("Failed to open terminal for writing!");
	}
	task->fileDescriptors[1] = fd;

	// create stderr
        errno = getConsoleDriver()->open(FileDescriptor::WRITE, fd);
	if (errno != NOERROR) {
		kernelPanic("Failed to open terminal for writing!");
	}
	task->fileDescriptors[2] = fd;
}

void UserTask::initialize(ZsosExeHeader *hdr) {
	unsigned int *tmp;
	char *text_address, *data_address;

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

        text_address = (char*)&_binary_example_zsx_start
		+ _binary_example_zsx_start.textPhys;

	m->setPrivPointer((void*)text_address);
	m->setLoader(&textPageLoader);

	int rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	m = new MemMapArea(
		VirtAddr((void*)hdr->dataVirt),
		VirtAddr((void*)(hdr->dataVirt + hdr->dataLength)));

        data_address = (char*)&_binary_example_zsx_start
		+ _binary_example_zsx_start.dataPhys;
	m->setPrivPointer((void*)data_address);
	m->setLoader(&dataPageLoader);

	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

        // BSS
	m = new MemMapArea(
		VirtAddr((void*)hdr->bssVirt),
		VirtAddr((void*)(hdr->bssVirt + hdr->bssLength)));
	m->setLoader(&zeroPageLoader);
	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}
        memmap->setBss(m);

        // STACK. Does not (yet) grow!
	m = new MemMapArea(
		VirtAddr((void*)(0xC0000000 - 0x100000)),
		VirtAddr((void*)(0xC0000000)));
	m->setLoader(&zeroPageLoader);
	rc = memmap->addAreaAt(m);
	if (rc != 0) {
		goto error;
	}

	initialize_stdstreams(this);

        fpu_used = true;
	return;

error:
	kernelPanic("Error in UserTask::initialize");
}

UserTask::~UserTask() {
}

void ____user_task_dispatch_new() {
        /* clear task switched as we are going fresh
           and doing fninit */
        clear_TS_in_CR0();
	__asm__ __volatile__(
		".globl ____user_task_dispatch_new_asm\n"
		"____user_task_dispatch_new_asm:\n\t"
		"popl %%gs\n\t"
        	"popl %%fs\n\t"
        	"popl %%es\n\t"
        	"popl %%ds\n\t"
                "fninit\n\t"
	        "iret\n\t"
	: : );
}

void UserTask::dispatch(uint32_t *saved_esp) {
        /* change TSS ESP0 to our kernel stack base */
	tssSegment.esp0 = kstack;

        /* set task switched for our new task... this is rather complicated,
           but it is run before our actual context switch. This will
           fail all FPU operations... */
        set_TS_in_CR0();

        /* Ok... change our contexts */
        Task::switchContexts(saved_esp);

        /* NEVER EXECUTE ANYTHING HERE, AS THE CODE IS RUN AFTER THE TASK HAS BEEN SWITCHED OFF,
           IN THE CONTEXT OF THE TASK THAT DISPATCHED US FOR THE LAST QUANTA. */
}

void UserTask::terminate() {
	extern Scheduler scheduler;
	setCurrentState(TERMINATED);
	scheduler.removeTask(this);
	if (process_id == 1) {
		kout << "Init killed, rebooting" << endl;
		reboot();
	}
	scheduler.schedule();
}

bool UserTask::handlePageFault(PageFaultInfo& f) {
	MemMapArea *m = memmap->findAreaByAddr(f.address);

	if (! m) {
                kout << "-- Page fault --" << endl;
		kout << "Invalid address: " << f.address << endl;
		print_kernel_state(*f.regs);
		printk("EIP: 0x%08x\n", f.eip);
		kernelPanic("User task killed...\n");
	}

	else {
		PageLoader *p = m->getLoader();
		if (p) {
			p->loadPage(this,
				m,
				f.write ? MemMapArea::W : MemMapArea::R,
				f.address);

			return true;
		}
		else {
			kernelPanic("Tried to address a memarea with no PageLoader");
		}
	}
}

void *UserTask::setBrk(void *n) {
	return memmap->setBrk(n);
}

void TssContents::setup() {
	memset(&tssSegment, 0, sizeof(tssSegment));
	ss0   = KERNEL_DATA_DESCRIPTOR;

        es =
	ss =
	ds =
	fs =
	gs = USER_DATA_DESCRIPTOR | 3;
        cs = USER_CODE_DESCRIPTOR | 3;
	eflags = 2;
	bitmap = 104;

	__asm__ __volatile__ ("mov %%cr3, %0" : "=a"(cr3) : );
}

void initialize_tasking() {
	tssSegment.setup();

	uint16_t tss_desc = TSS_DESCRIPTOR;
	__asm__ __volatile__ ("ltr %0" : : "r"(tss_desc));
}

void (*_save_FPU_state)(uint8_t *ptr)    = FPU_save_with_FNSAVE;
void (*_restore_FPU_state)(uint8_t *ptr) = FPU_restore_with_FRSTOR;

void UserTask::handleNMException() {
       clear_TS_in_CR0();

        _restore_FPU_state(getFpuStatePtr());
       fpu_used = true;
}

void UserTask::prepareContextSwitch() {
       if (fpu_used) {
           _save_FPU_state(getFpuStatePtr());
           fpu_used = false;
       }
}

