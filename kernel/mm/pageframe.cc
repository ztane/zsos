#include <kernel/mm/pageframe.hh>
#include <kernel/multiboot.hh>
#include <kernel/mm/memarea.hh>
#include <iostream>

PageFrameTable frames;

extern char _BOOT_HEAP_END;
extern char g_code;


void initializePageFrameTable(
		MultibootInfo& boot_info,
		Allocator& alloc
	)
{
	uintptr_t lastPageFrameNumber =
		boot_info.get_max_ram_address() / 0x1000l + 1l;

	frames.initialize(lastPageFrameNumber, alloc);

        int ct = boot_info.number_of_mmap_entries();

        for (int i = 0; i < ct; i++)
        {
                const MultibootMMapInfo& inf =
                        boot_info.mmap_entry(i);

		// does not handle alignment...
		uintptr_t start = inf.get_base() / 0x1000;
		size_t length = inf.get_length() / 0x1000;
		PageFrame *startFrame = frames.getFrameByNumber(start);

		frames.setFlagsRange(startFrame, length, PageFrame::IS_RAM);
		frames.clearFlagsRange(startFrame, length, PageFrame::LOCKED);
        }

	uintptr_t kstart  = ((uint32_t)&g_code - 0xC0000000) / 0x1000;
	uintptr_t kend    = ((uint32_t)&_BOOT_HEAP_END - 0xC0000000) / 0x1000;
	uintptr_t klength = kend - kstart + 1;

	frames.setFlagsRange(frames.getFrameByNumber(kstart),
		klength, PageFrame::LOCKED | PageFrame::KERNEL);

        int nmods = boot_info.number_of_modules();
        for (int i = 0; i < nmods; i ++) {
		const MultibootModuleInfo& m = boot_info.get_module(i);

		uintptr_t modstart = m.get_base()             / 0x1000;
		uintptr_t modlen   = (m.get_length() + 0xFFF) / 0x1000;

		frames.setFlagsRange(frames.getFrameByNumber(modstart),
			modlen, PageFrame::LOCKED | PageFrame::KERNEL);
        }

	DMAMemory.init(0, 0x1000);
	NormalMemory.init(0x1000,   0x30000 -  0x1000);
	HighMemory.init(0x30000,  0x100000 - 0x30000);
}

void PageFrameTable::acquireRange(PageFrame* start, size_t length)
{
	PageFrame* end = start + length;
	for ( ; start < end; start ++) {
		start->acquire();
	}
}

void PageFrameTable::releaseRange(PageFrame* start, size_t length)
{
	PageFrame* end = start + length;
	for ( ; start < end; start ++) {
		start->release();
	}
}

void PageFrameTable::setFlagsRange(PageFrame* start, size_t length, int32_t flag)
{
	PageFrame* end = start + length;
	for ( ; start < end; start ++) {
		start->setFlag(flag);
	}
}

void PageFrameTable::clearFlagsRange(PageFrame* start, size_t length, int32_t flag)
{
        PageFrame* end = start + length;
	for ( ; start < end; start ++) {
		start->clearFlag(flag);
	}
}
