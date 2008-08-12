#include "kernel/mm/datapageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"


int DataPageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	uint32_t offset   = addr - m->getBase();

	char *physPos     = (char*)m->getPrivPointer();
	physPos          += offset;
	pageaddr_t phys_a = pageaddr_t::fromVirtual(physPos);		

	PageFlags flags = PageFlags::PRESENT 
		| PageFlags::USER | PageFlags::READWRITE;

        mapPage(page_directory, addr, &page_frames.getByFrame(phys_a), flags);
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