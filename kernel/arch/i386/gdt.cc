#include "memory.hh"
#include "task.hh"
#include "printk.h"

#define N_SEGMENT_DESCRIPTORS 5

#define CODE_ACCESS	       DSC_ACCESS_PRESENT    | \
	DSC_ACCESS_READABLE  | DSC_ACCESS_EXECUTABLE | \
	DSC_ACCESS_DATA_CODE

#define DATA_ACCESS	       DSC_ACCESS_PRESENT    | \
	DSC_ACCESS_WRITABLE  | DSC_ACCESS_DATA_CODE

#define FLAGS DSC_FLAG_PAGE_GRAN | DSC_FLAG_DEFAULT_32

extern class TssContents tssSegment;

SegmentDescriptor GDT[] = {
	SegmentDescriptor(),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		CODE_ACCESS, FLAGS),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS, FLAGS),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		CODE_ACCESS | DSC_ACCESS_RING_3, FLAGS),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS | DSC_ACCESS_RING_3, FLAGS),

	SegmentDescriptor((unsigned long)&tssSegment, 103,
		DSC_ACCESS_PRESENT | DSC_ACCESS_IS_TSS, 0),

	// TLS segments
	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS, FLAGS),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS, FLAGS),

	SegmentDescriptor(0x00000000, 0xFFFFFFFF,
		DATA_ACCESS, FLAGS),

};

void get_gdt_location(void* & location, int& limit)
{
	location = (void*)GDT;
	limit = (int)(sizeof(GDT) - 1);
}

static struct _gdtr {
        unsigned char limit_7_0;
        unsigned char limit_15_8;
        unsigned char base_7_0;
        unsigned char base_15_8;
        unsigned char base_23_16;
        unsigned char base_31_24;
} gdtr;

void init_gdt()
{
        int limit = sizeof(GDT) - 1;
        unsigned long base = (unsigned long)GDT;

        gdtr.limit_7_0  =  limit       & 0xff;
        gdtr.limit_15_8 = (limit >> 8) & 0xff;

        gdtr.base_7_0   =  base        & 0xff;
        gdtr.base_15_8  = (base >> 8)  & 0xff;
        gdtr.base_23_16 = (base >> 16) & 0xff;
        gdtr.base_31_24 = (base >> 24) & 0xff;

        /* load global descriptor table to GDTR */
        __asm__ __volatile__ ("lgdt (%0)" : : "r" ((void *) &gdtr));
}

ostream& operator<<(ostream& os, SegmentDescriptor& desc) {
	ios_base::fmtflags f = os.setf(ios_base::hex);
	os << "Base: " << desc.get_base();
	os << ", limit " << desc.get_limit();
	os << ", flags " << desc.get_flags();
	os << ". access" << desc.get_access() << endl;
	os.setf(f);
	return os;
}

