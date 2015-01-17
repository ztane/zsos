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
	: Task(name, state, priority), baseAddress(0)
{
	for (int i = 0; i < MAX_FILEDES; i++) {
		fileDescriptors[i] = NULL;
	}
}

void UserTask::enable_io() {
	// set IOPL to 3...
	unsigned int *tmp = (uint32_t*)kstack - 3;
	*tmp |= 3 << 12;
}

void UserTask::setUserStackPointer(uint32_t sp) {
	uint32_t *tmp = (uint32_t*)kstack - 2;
	*tmp = sp;
}

void UserTask::setEntryPointer(uint32_t entry) {
	uint32_t *tmp = (uint32_t*)kstack - 5;
	*tmp = entry;
        entryPointer = entry;
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

static char *copyStackStr(char *stack, char *str) {
        uint32_t len = strlen(str) + 1;
        stack -= len;
        memcpy(stack, str, len);
        return stack;
}

static char *alignStackPtr(char *stack, int align) {
        uint32_t mask = (uint32_t)align - 1;
        mask = ~mask;
        stack = (char *)((uint32_t)stack & mask);
        return stack;
}

static char *storeStackValue(char *stack, uint32_t value) {
        stack -= sizeof(uint32_t);
        *(uint32_t*)stack = value;
        return stack;
}

void UserTask::buildInitialKernelStack() {
	unsigned int *stack;

 	// build kernel stack
	stack    = (unsigned int *)(kernel_stack + sizeof(kernel_stack));
	kstack   = (unsigned int)stack;

	*--stack = USER_DATA_DESCRIPTOR + 3;
	*--stack = 0;      // esp;
        *--stack = 0x0202; // flags

        *--stack = USER_CODE_DESCRIPTOR + 3;
        *--stack = 0;      // entry point

        *--stack = USER_DATA_DESCRIPTOR + 3; // ds
        *--stack = USER_DATA_DESCRIPTOR + 3; // es
        *--stack = USER_DATA_DESCRIPTOR + 3; // fs
        *--stack = USER_DATA_DESCRIPTOR + 3; // gs

	// return here after switching stacks...
	*--stack = (uint32_t)____user_task_dispatch_new_asm;

        *--stack = 0;    // EAX
        *--stack = 0;    // ECX
        *--stack = 0;    // EDX
        *--stack = 0;    // EBX
        *--stack = 0;    // ESP
        *--stack = 0;    // EBP
        *--stack = 0;    // ESI
        *--stack    = (uint32_t)this;    // EDI, for use in init routine store this here.

	esp = (unsigned int)stack;
}

void UserTask::initialize(ZsosExeHeader *hdr) {
	header = hdr;
        buildInitialKernelStack();
}

// store value first (to upper address!)
#define ELF_TAG(tag, value)                \
    stack = storeStackValue(stack, value); \
    stack = storeStackValue(stack, tag);

#define AT_NULL         0               /* End of vector */
#define AT_IGNORE       1               /* Entry should be ignored */
#define AT_EXECFD       2               /* File descriptor of program */
#define AT_PHDR         3               /* Program headers for program */
#define AT_PHENT        4               /* Size of program header entry */
#define AT_PHNUM        5               /* Number of program headers */
#define AT_PAGESZ       6               /* System page size */
#define AT_BASE         7               /* Base address of interpreter */
#define AT_FLAGS        8               /* Flags */
#define AT_ENTRY        9               /* Entry point of program */
#define AT_NOTELF       10              /* Program is not ELF */
#define AT_UID          11              /* Real uid */
#define AT_EUID         12              /* Effective uid */
#define AT_GID          13              /* Real gid */
#define AT_EGID         14              /* Effective gid */
#define AT_PLATFORM     15		/* AT platform */
#define AT_CLKTCK       17              /* Frequency of times() */
#define AT_RANDOM       25              /* random shit */

typedef struct
{
  uint32_t a_type;              /* Entry type */
  union
    {
      uint32_t a_val;           /* Integer value */
      /* We use to have pointer elements added here.  We cannot do that,
         though, since it does not work when using 32-bit definitions
         on 64-bit platforms and vice versa.  */
    } a_un;
} Elf32_auxv_t;


void UserTask::initializeUserStack() {
	char *stack = (char *)0xC0000000;
	stack = storeStackValue(stack, 0xDEADBEEF);
	stack = storeStackValue(stack, 0x666B00B2);
	stack = storeStackValue(stack, 0x42424242);
	char *random = stack = storeStackValue(stack, 0x12345678);
	stack = storeStackValue(stack, 0);

	char *dumpTags = stack = copyStackStr(stack, "LD_SHOW_AUXV=1");
	char *dumpTags2 = stack = copyStackStr(stack, "LD_DEBUG=all");
	char *argv2 = stack = copyStackStr(stack, "Hello world");
	char *argv1 = stack = copyStackStr(stack, "/bin/echo");
	char *argv0 = stack = copyStackStr(stack, "/lib/ld-linux.so.2");
	char *platform = stack = copyStackStr(stack, "i686");

        // TODO: auxv map!
	stack = alignStackPtr(stack, 16);
	ELF_TAG(AT_NULL, 0);

	if (elfHeader) {
		ELF_TAG(AT_PHDR,   (baseAddress + elfHeader->phOff).toInteger());
		ELF_TAG(AT_PHENT,  elfHeader->phEntSize);
		ELF_TAG(AT_PHNUM,  elfHeader->phNum);
		ELF_TAG(AT_PAGESZ, Paging::PAGE_SIZE);
		ELF_TAG(AT_BASE,   baseAddress.toInteger());
		ELF_TAG(AT_ENTRY,  (uint32_t)entryPointer);
	}
	else {
		ELF_TAG(AT_NOTELF, 1);
	}

	ELF_TAG(AT_UID, uid);
	ELF_TAG(AT_UID, gid);
	ELF_TAG(AT_UID, euid);
	ELF_TAG(AT_UID, egid);
	ELF_TAG(AT_CLKTCK, 100);
	ELF_TAG(AT_PLATFORM, (uint32_t)platform);
	ELF_TAG(AT_RANDOM, (uint32_t)random);

	// envp
        stack = storeStackValue(stack, 0);
	stack = storeStackValue(stack, (uint32_t)dumpTags);
	stack = storeStackValue(stack, (uint32_t)dumpTags2);

	// argv end
        stack = storeStackValue(stack, 0);
        stack = storeStackValue(stack, (uint32_t)argv2);
        stack = storeStackValue(stack, (uint32_t)argv1);
        stack = storeStackValue(stack, (uint32_t)argv0);
        stack = storeStackValue(stack, 3);
        setUserStackPointer((uint32_t)stack);
}

void UserTask::buildMaps() {
     ZsosExeHeader *hdr = header;
     if (hdr != NULL) {
         char *text_address, *data_address;

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
        setEntryPointer(hdr->entryPoint);
    }
    else if (elfHeader != NULL) {
 	memmap = new MemMap();

        kout << "loadable elf program headers" << endl;

	VirtAddr base = VirtAddr((void*)0x400000);
	baseAddress = base;
	VirtAddr heapStart = base;

        for (int i = 0; i < elfHeader->phNum; i++) {
		const Elf32ProgramHeader& hdr = elfHeader->programHeader(i);
		if (hdr.isLoadable()) {
			printk("Start addr %08x, flags %08x\n", hdr.p_vaddr, hdr.p_flags);
			VirtAddr areaBase = (base + hdr.p_vaddr).getPageStart();
			VirtAddr areaEnd = (base + hdr.p_vaddr + hdr.p_memsz).getNextPage();
			if (areaEnd > heapStart) {
				heapStart = areaEnd;
			}

			MemMapArea *m = new MemMapArea(areaBase, areaEnd);
			if (hdr.p_flags & 0x1) {
				if (hdr.p_flags & 0x2) {
					printk("Writable text segments not supported\n");
					goto error;
				}

				if (hdr.p_memsz != hdr.p_filesz) {
					printk("Text segments with zeroes not supported\n");
				}

				m->setLoader(&textPageLoader);
			}
			else {
				m->setLoader(&dataPageLoader);

				if (hdr.p_memsz != hdr.p_filesz) {
					VirtAddr zeroStart = base + hdr.p_vaddr + hdr.p_filesz;
					m->setZeroStart(zeroStart);
				}
			}

			VirtAddr loadFrom = (VirtAddr(elfHeader) + hdr.p_offset).getPageStart();
			m->setPrivPointer(loadFrom.toPointer());

			if (memmap->addAreaAt(m) != 0) {
				goto error;
			}

			printk("area: start %08x, end %08x, zeroStart: %08x\n",
				m->getBase().toInteger(), m->getEnd().toInteger(), m->getZeroStart().toInteger());

		}
        }

	MemMapArea *m = new MemMapArea(heapStart, heapStart);
	m->setLoader(&zeroPageLoader);
	if (memmap->addAreaAt(m) != 0) {
		goto error;
	}
        memmap->setBss(m);

	// STACK. not growing yet
	m = new MemMapArea(
		VirtAddr((void*)(0xC0000000 - 0x100000)),
		VirtAddr((void*)(0xC0000000)));
	m->setLoader(&zeroPageLoader);
	if (memmap->addAreaAt(m) != 0) {
		goto error;
	}

        setEntryPointer((base + elfHeader->entry).toInteger());
	memmap->dumpMemMap();
//        kernelPanic("Cannot continue...");
    }

    else {
        goto error;
    }

    initialize_stdstreams(this);
    fpu_used = true;

    initializeUserStack();

    return;

error:
    kernelPanic("Error in UserTask::initialize");
}

UserTask::~UserTask() {
}

// called from ASM in the task context
extern "C" void _finalizeUserTaskInit(UserTask *task) {
        task->buildMaps();
}


void ____user_task_dispatch_new() {
        /* clear task switched as we are going fresh
           and doing fninit */
        clear_TS_in_CR0();
	__asm__ __volatile__(
		".globl ____user_task_dispatch_new_asm\n"
		"____user_task_dispatch_new_asm:\n"
                "        pushal\n"
                // EDI (the last pushed) contains the Task pointer
                "        call _finalizeUserTaskInit\n"
                "        popal\n"
                "        xor %%edi, %%edi\n"
		"        popl %%gs\n"
        	"        popl %%fs\n"
        	"        popl %%es\n"
        	"        popl %%ds\n"
                "        fninit\n"
	        "        iret\n"
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
           IN THE CONTEXT OF THE TASK THAT DISPATCHED US FOR THE LAST QUANTUM. */
}

void UserTask::terminate() {
	extern Scheduler scheduler;
	setCurrentState(TERMINATED);
	scheduler.removeTask(this);
	if (process_id == 1) {
		kout << "Init killed, rebooting" << endl;
                kernelPanic("Not rebooting");
		reboot();
	}
	scheduler.schedule();
}

bool UserTask::handlePageFault(PageFaultInfo& f) {
	MemMapArea *m = memmap->findAreaByAddr(f.address);

	if (! m) {
                kout << "-- Page fault --" << endl;
		kout << "Invalid address: " << f.address.toPointer() << endl;
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

void UserTask::initialize(ElfExeHeader *hdr) {
	header = NULL;
	elfHeader = hdr;
	buildInitialKernelStack();
}
