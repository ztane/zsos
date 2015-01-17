#include "kernel/mm/filepageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"
#include "kernel/printk.h"

static void *getPageStart(void *addr) {
	intptr_t tmp = (intptr_t)addr;
	tmp &= ~(intptr_t)0xFFF;
	return (void*)tmp;
}

int FilePageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	VirtAddr page = addr.getPageStart();
	VirtAddr zeroStartPage = m->getZeroStart().getPageStart();

	// the page contains data from the dataloader, that is, at least
	// some of its bytes
        PageFrame *frm;
	if (page <= zeroStartPage && m->getFile()) {
	        frm = MM::allocatePage();
        	if (! frm) {
                	kernelPanic("Not enough space for zero pages...\n");
	        }

		uint64_t offset   = page - m->getBase();
		offset += ((uint64_t)m->getOffset()) << Paging::PAGE_SHIFT;

		size_t read = 0;
		ErrnoCode rc = m->getFile()->read(frm->toLinear(), Paging::PAGE_SIZE, FileOffset(offset), read);
		if (rc != 0) {
			printk("Failed to read stuff from file in FPL!!!");
		}
		if (page == zeroStartPage) {
			// this page also contains zeroed bytes
			size_t zeroFrom = m->getZeroStart() - zeroStartPage;
			memset((char*)frm->toLinear() + zeroFrom, 0, Paging::PAGE_SIZE - zeroFrom);
		}
	}
	else {
		// it is a zero-only page!
		frm = MM::allocateZeroPage();
	}

        PageFlags flags = PageFlags::PRESENT | PageFlags::USER;
        if (access & MemMapArea::W) {
                flags.setRW(true);
        }

        mapPage(page_directory, addr, frm, flags);
        return 0;
}

int FilePageLoader::swapIn
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	kernelPanic("FPL swapIn\n");
}

int FilePageLoader::doCow
	(UserTask *t, MemMapArea *m, VirtAddr addr)
{
	kernelPanic("FPL doCow\n");
}

FilePageLoader filePageLoader;
