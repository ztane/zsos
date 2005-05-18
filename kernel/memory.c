#include "memory.h"

#define DSC_FLAG_PAGE_GRAN   0x80
#define DSC_FLAG_DEFAULT_32  0x40

#define DSC_ACCESS_PRESENT   0x80
#define DSC_ACCESS_PRIV_MASK 0x60

#define DSC_ACCESS_RING_0    0x00
#define DSC_ACCESS_RING_1    0x20
#define DSC_ACCESS_RING_2    0x40
#define DSC_ACCESS_RING_3    0x60

#define DSC_ACCESS_DATA_CODE   0x10

#define DSC_ACCESS_EXECUTABLE  0x08

#define DSC_ACCESS_EXPAND_DOWN 0x04
#define DSC_ACCESS_CONFORMING  0x04

#define DSC_ACCESS_WRITABLE    0x02
#define DSC_ACCESS_READABLE    0x02

#define DSC_ACCESS_ACCESSED    0x01

void get_gd_values(gdt_descriptor *desc, gdt_info *info)
{
	info->limit  = desc->limit_7_0;
	info->limit += (unsigned)(desc->limit_15_8)   << 8;
	info->limit += ((unsigned)(desc->flags) & 15) << 16;

	info->base   = desc->base_7_0;
	info->base  += (unsigned)(desc->base_15_8)    << 8;
	info->base  += (unsigned)(desc->base_23_16)   << 16;
	info->base  += (unsigned)(desc->base_31_24)   << 24;
	
	info->access = desc->access;
	info->flags  = desc->flags & 0xF0;

	if (desc->flags & DSC_FLAG_PAGE_GRAN) 
	{
		info->limit <<= 12;
		info->limit  += 0xFFF;
	}
}

void set_gd_values(gdt_info *info, gdt_descriptor *desc)
{
	if (info->flags & DSC_FLAG_PAGE_GRAN) {
		info->limit >>= 12;
	}

	desc->limit_7_0  =  info->limit        & 0xFF;
	desc->limit_15_8 = (info->limit >> 8)  & 0xFF;
	desc->flags      = (info->limit >> 16) & 0x0F;

	desc->base_7_0   =  info->base         & 0xFF;	
	desc->base_15_8  = (info->base >> 8)   & 0xFF;	
	desc->base_23_16 = (info->base >> 16)  & 0xFF;	
	desc->base_31_24 = (info->base >> 24)  & 0xFF;	

	desc->access     = info->access & 0xFF;
	desc->flags     |= info->flags  & 0xF0;
}




