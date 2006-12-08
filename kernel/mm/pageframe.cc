#include <kernel/mm/pageframe.hh>
#include <kernel/multiboot.hh>
#include <iostream>

PageFrameTable global_page_frame_table;

extern char _BOOT_HEAP_END;
extern char g_code;


void initialize_page_frame_table(
		const MultibootInfo& boot_info, 
		Allocator& alloc
	)
{
	pageaddr_t last_page = 
		boot_info.get_max_ram_address() / 0x1000l + 1l;
	
	global_page_frame_table.initialize(last_page, alloc);

        int ct = boot_info.number_of_mmap_entries();

        for (int i = 0; i < ct; i++)
        {
                const MultibootMMapInfo& inf =
                        boot_info.mmap_entry(i);
		

		// does not handle alignment...
		pageaddr_t start = inf.get_base() / 0x1000;
		size_t length = inf.get_length() / 0x1000;
		kout << "Mapping as ram: " << length << " pages ";
		kout << " at " << start << "." << endl;
		
		global_page_frame_table.set_flags_range(
			start, length, PageFrame::IS_RAM);

		global_page_frame_table.clear_flags_range(
			start, length, PageFrame::LOCKED);
        }

	pageaddr_t kstart  = ((uint32_t)&g_code - 0xC0000000) / 0x1000;
	pageaddr_t kend    = ((uint32_t)&_BOOT_HEAP_END - 0xC0000000) / 0x1000;
	pageaddr_t klength = kend - kstart + 1;
	kout << "Reserving " << klength << " pages at " << kstart << " for kernel." << endl;

	global_page_frame_table.set_flags_range(kstart, klength, PageFrame::LOCKED | PageFrame::KERNEL);
}

void PageFrameTable::acquire_range(pageaddr_t start, size_t length) 
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].acquire();
	}
}

void PageFrameTable::release_range(pageaddr_t start, size_t length) 
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].release();
	}	
}

void PageFrameTable::set_flags_range(pageaddr_t start, size_t length, int32_t flag) 
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].set_flag(flag);
	}	
}

void PageFrameTable::clear_flags_range(pageaddr_t start, size_t length, int32_t flag) 
{
	int end = start + length;
	for (int i = start; i < end; i ++) {
		page_frames[i].clear_flag(flag);
	}	
}
