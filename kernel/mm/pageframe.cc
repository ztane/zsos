#include <kernel/mm/pageframe.hh>
#include <kernel/multiboot.hh>
#include <iostream>

PageFrameTable page_frames;

extern char _BOOT_HEAP_END;
extern char g_code;


void initializePageFrameTable(
		MultibootInfo& boot_info, 
		Allocator& alloc
	)
{
	pageaddr_t last_page =
		boot_info.get_max_ram_address() / 0x1000l + 1l;

	page_frames.initialize(last_page, alloc);

        int ct = boot_info.number_of_mmap_entries();

        for (int i = 0; i < ct; i++)
        {
                const MultibootMMapInfo& inf =
                        boot_info.mmap_entry(i);


		// does not handle alignment...
		pageaddr_t start = inf.get_base() / 0x1000;
		size_t length = inf.get_length() / 0x1000;

		page_frames.setFlagsRange(
			start, length, PageFrame::IS_RAM);

		page_frames.clearFlagsRange(
			start, length, PageFrame::LOCKED);
        }

	pageaddr_t kstart  = ((uint32_t)&g_code - 0xC0000000) / 0x1000;
	pageaddr_t kend    = ((uint32_t)&_BOOT_HEAP_END - 0xC0000000) / 0x1000;
	pageaddr_t klength = kend - kstart + 1;
	// kout << "Reserving " << klength << " pages at " << kstart << " for kernel." << endl;

	page_frames.setFlagsRange(kstart, klength, PageFrame::LOCKED | PageFrame::KERNEL);

        int nmods = boot_info.number_of_modules();
        for (int i = 0; i < nmods; i ++) {
		const MultibootModuleInfo& m = boot_info.get_module(i);

		pageaddr_t modstart = m.get_base()             / 0x1000;
		pageaddr_t modlen   = (m.get_length() + 0xFFF) / 0x1000;

                // kout << "Reserving" << modlen << " pages at " << modstart << " for module " << m.get_string() << endl;
		page_frames.setFlagsRange(modstart, modlen, PageFrame::LOCKED | PageFrame::KERNEL);
        }

}

void PageFrameTable::acquireRange(pageaddr_t start, size_t length)
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].acquire();
	}
}

void PageFrameTable::releaseRange(pageaddr_t start, size_t length)
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].release();
	}
}

void PageFrameTable::setFlagsRange(pageaddr_t start, size_t length, int32_t flag)
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].setFlag(flag);
	}
}

void PageFrameTable::clearFlagsRange(pageaddr_t start, size_t length, int32_t flag)
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].clearFlag(flag);
	}
}
