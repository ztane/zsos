#include "kernel/mm/zeropageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"


int ZeroPageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	PageFrame *frm = MM::allocateZeroPage();

        if (! frm) {
        	kernelPanic("Not enough space for zero pages...\n");
        }

	PageFlags flags = PageFlags::PRESENT | PageFlags::USER;
	if (access & MemMapArea::W) {
		flags = 
			PageFlags::PRESENT | PageFlags::USER | 
			PageFlags::READWRITE;
	}
 
        mapPage(page_directory, addr, frm, flags);
	return 0;
}

int ZeroPageLoader::swapIn
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	kernelPanic("ZPL swapIn\n");
}

int ZeroPageLoader::doCow
	(UserTask *t, MemMapArea *m, VirtAddr addr)
{
	kernelPanic("ZPL doCow\n");
}

ZeroPageLoader zeroPageLoader;
