#include "kernel/mm/datapageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"


void *getPageStart(void *addr) {
	intptr_t tmp = (intptr_t)addr;
	tmp &= ~(intptr_t)0xFFF;
	return (void*)tmp;
}

int DataPageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	VirtAddr page = addr.getPageStart();
	VirtAddr zeroStartPage = m->getZeroStart().getPageStart();

	// the page contains data from the dataloader, that is, at least
	// some of its bytes
        PageFrame *frm;
	if (page <= zeroStartPage) {
	        frm = MM::allocatePage();
        	if (! frm) {
                	kernelPanic("Not enough space for zero pages...\n");
	        }

		uint32_t offset   = page - m->getBase();
		void *from = (char*)m->getPrivPointer() + offset;
		memcpy(frm->toLinear(), from, Paging::PAGE_SIZE);

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

int DataPageLoader::swapIn
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	kernelPanic("DPL swapIn\n");
}

int DataPageLoader::doCow
	(UserTask *t, MemMapArea *m, VirtAddr addr)
{
	kernelPanic("DPL doCow\n");
}

DataPageLoader dataPageLoader;
