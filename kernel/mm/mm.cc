#include <string.h>

#include "kernel/interrupt.hh"
#include "kernel/mm/mm.hh"
#include "kernel/mm/memarea.hh"

PageFrame * volatile MM::global0Page = 0;

PageFrame *MM::allocateZeroPage() {
	PageFrame *f = NormalMemory.allocatePage();
	void *addr = f->getPageAddr().toVirtual();
	memset(addr, 0, 4096);

	return f;
}

PageFrame *MM::getGlobalZeroPage() {
	if (global0Page != 0) {
		global0Page->acquire();
		return global0Page;
	}
	
	bool save = disableInterrupts();

	if (global0Page == 0) {
		global0Page = allocateZeroPage();
		global0Page->setFlag(PageFrame::COW);
	}
	global0Page->acquire();

	enableInterruptsIf(save);
	return global0Page;
}
