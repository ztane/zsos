#include "kernel/mm/datapageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"


int DataPageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	uint32_t offset   = addr - m->getBase();

	char *physPos     = (char*)m->getPrivPointer();
	physPos          += offset;
	uintptr_t phys_a = frameNumberFromLinear(physPos);

	PageFlags flags = PageFlags::PRESENT
		| PageFlags::USER | PageFlags::READWRITE;

        mapPage(page_directory, addr, frames.getFrameByNumber(phys_a), flags);
	return 0;
}

int DataPageLoader::swapIn
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	kernelPanic("ZPL swapIn\n");
}

int DataPageLoader::doCow
	(UserTask *t, MemMapArea *m, VirtAddr addr)
{
	kernelPanic("ZPL doCow\n");
}

DataPageLoader dataPageLoader;
