#include "memory"

#define N_SEGMENT_DESCRIPTORS 5

#define CODE_ACCESS	       DSC_ACCESS_PRESENT    | \
	DSC_ACCESS_READABLE  | DSC_ACCESS_EXECUTABLE | \
	DSC_ACCESS_DATA_CODE

#define DATA_ACCESS	       DSC_ACCESS_PRESENT    | \
	DSC_ACCESS_WRITABLE  | DSC_ACCESS_DATA_CODE

#define FLAGS DSC_FLAG_PAGE_GRAN | DSC_FLAG_DEFAULT_32 

SegmentDescriptor GDT[] = {
	SegmentDescriptor(),
	SegmentDescriptor(0x00000000, 0xFFFFFFFF, 
		CODE_ACCESS, FLAGS),
	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS, FLAGS),
};

void get_gdt_location(void* & location, int& limit)
{
	location = (void*)GDT;
	limit = (int)(sizeof(GDT) - 1);
}


