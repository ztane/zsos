// -*- C++ -*-

#ifndef __MEMORY_INCLUDED__
#define __MEMORY_INCLUDED__

#define KERNEL_CODE_DESCRIPTOR 0x08
#define KERNEL_DATA_DESCRIPTOR 0x10
#define   USER_CODE_DESCRIPTOR 0x18
#define   USER_DATA_DESCRIPTOR 0x20
#define         TSS_DESCRIPTOR 0x28


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
#define DSC_ACCESS_IS_TSS      0x09
#define DSC_ACCESS_IS_BUSY     0x02


class SegmentDescriptor {
protected:
        unsigned char limit_7_0;
        unsigned char limit_15_8;

        unsigned char base_7_0;
        unsigned char base_15_8;
        unsigned char base_23_16;
        unsigned char access;

        unsigned char flags;
        unsigned char base_31_24;
public:
	SegmentDescriptor() {
		limit_7_0  = 0;
		limit_15_8 = 0;
		base_7_0   = 0;
		base_15_8  = 0;
		base_23_16 = 0;
		access     = 0;
		flags      = 0;
		base_31_24 = 0;
	}
	SegmentDescriptor(unsigned long base, unsigned long limit, 
		int access, int flags) 
	{
		set_flags(flags);
		set_access(access);
		set_base(base);
		set_limit(limit, flags & 0x80);
	}	

	void set_limit(unsigned long limit, bool page_gran = true)
	{
        	if (page_gran) {
                	limit >>= 12;
	        }

	        limit_7_0  =  limit        & 0xFF;
	       	limit_15_8 = (limit >> 8)  & 0xFF;

	        flags &= 0xF0; 
		flags |= (limit >> 16) & 0x0F;
	}

	void set_base(unsigned long base) {
	        base_7_0   =  base         & 0xFF;
	        base_15_8  = (base >> 8)   & 0xFF;
	        base_23_16 = (base >> 16)  & 0xFF;
        	base_31_24 = (base >> 24)  & 0xFF;
	}

	void set_flags(int _flags) {
		flags &= 0xF;
		flags |= _flags & 0xF0;
	}

	void set_access(int _access) {
		access = _access;
	}

	unsigned long get_limit() {
	        unsigned long limit = limit_7_0;
	        limit += (int)limit_15_8 << 8;
	        limit += ((int)flags & 0xF) << 16;

        	if (flags & 0x80) {
                	limit <<= 12;
			limit  += 0xFFF;
	        }

		return limit;
	}

	unsigned long get_base() {
		unsigned long base = base_7_0;
		base |= (unsigned int)base_15_8  << 8;
		base |= (unsigned int)base_23_16 << 8;
		base |= (unsigned int)base_31_24 << 8;
		return base;
	}
	
	int get_access() {
		return access;
	}

	int get_flags() {
	        return flags & 0xF0;
	}
} __attribute__ ((packed));

SegmentDescriptor *get_descriptor(int number);
void init_gdt();

class PhysAddr {
private:
	void *ptr;
public:
	PhysAddr() {
		ptr = 0;
	}

	static const PhysAddr create(void *ptr) {
		PhysAddr rv;
		rv.set(ptr);
		return rv;
	}

	static const PhysAddr create(unsigned long addr) {
		PhysAddr rv;
		rv.set(addr);
		return rv;
	}

	void set(unsigned long addr) {
		ptr = (void*)addr;
	}

	void set(void *ptr) {
		this->ptr = ptr;
	}

	unsigned long get_value() const {
		return (unsigned long)ptr;
	}
};

#endif