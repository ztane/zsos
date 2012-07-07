#include "kernel/mm/textpageloader.hh"
#include "kernel/mm/mm.hh"
#include "kernel/panic.hh"


int TextPageLoader::loadPage
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	uint32_t offset   = addr - m->getBase();

	char *physPos     = (char*)m->getPrivPointer();
	physPos          += offset;
	uintptr_t  phys_a = frameNumberFromLinear(physPos);

	if (access & MemMapArea::W) {
		kout << "Tryint to write to text area " << phys_a << endl;
		return -1;
	}

	PageFlags flags = PageFlags::PRESENT | PageFlags::USER;

        mapPage(page_directory, addr, frames.getFrameByNumber(phys_a), flags);
	return 0;
}

int TextPageLoader::swapIn
	(UserTask *t, MemMapArea *m, MemMapArea::Prot access, VirtAddr addr)
{
	kernelPanic("ZPL swapIn\n");
}

int TextPageLoader::doCow
	(UserTask *t, MemMapArea *m, VirtAddr addr)
{
	kernelPanic("ZPL doCow\n");
}

TextPageLoader textPageLoader;
