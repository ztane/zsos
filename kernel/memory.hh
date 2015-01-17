// -*- C++ -*-

#ifndef __MEMORY_INCLUDED__
#define __MEMORY_INCLUDED__

#include <inttypes.h>
#include "kernel/printk.h"
#include <iostream>

#define KERNEL_CODE_DESCRIPTOR 0x08
#define KERNEL_DATA_DESCRIPTOR 0x10
#define   USER_CODE_DESCRIPTOR 0x18
#define   USER_DATA_DESCRIPTOR 0x20
#define         TSS_DESCRIPTOR 0x28

static const int TLS_DESCRIPTOR_1 = 0x30;
static const int TLS_DESCRIPTOR_2 = 0x38;
static const int TLS_DESCRIPTOR_3 = 0x40;
static const int TLS_ENTRY_MIN    = TLS_DESCRIPTOR_1 / 8;
static const int N_TLS_ENTRY      = 3;
static const int TLS_ENTRY_MAX    = TLS_ENTRY_MIN + N_TLS_ENTRY - 1;


#define DSC_FLAG_PAGE_GRAN   0x80
#define DSC_FLAG_DEFAULT_32  0x40
#define DSC_FLAG_LONG_MODE   0x20
#define DSC_FLAG_EXTRA       0x10

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
	union {
		struct {
			uint16_t limit_15_0;
			uint16_t base_15_0;
		};
		uint32_t low;
	};

	union {
		struct {
		        uint8_t  base_23_16;
		        uint8_t  access;

		        uint8_t  flags;
		        uint8_t  base_31_24;
		};
		uint32_t high;
	};

public:
	SegmentDescriptor() {
		limit_15_0 = 0;
		base_15_0  = 0;
		base_23_16 = 0;
		access     = 0;
		flags      = 0;
		base_31_24 = 0;
	}
	SegmentDescriptor(unsigned long base, unsigned long limit,
		int access, int flags) {
		set_flags(flags);
		set_access(access);
		set_base(base);
		set_limit(limit, flags & 0x80);
	}

	void set_limit(unsigned long limit, bool page_gran = true) {
        	if (page_gran) {
                	limit >>= 12;
		}

	        limit_15_0  = limit & 0xFFFF;

	        flags &= 0xF0;
		flags |= (limit >> 16) & 0x0F;
	}

	void set_base(unsigned long base) {
	        base_15_0  = base & 0xFFFF;
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

	uint32_t get_limit(bool adjust = true) {
	        uint32_t limit = limit_15_0;
	        limit += ((uint32_t)flags & 0xF) << 16;

        	if (flags & 0x80 && adjust) {
                	limit <<= 12;
			limit  += 0xFFF;
	        }

		return limit;
	}

	uint32_t get_base() {
		uint32_t base = base_15_0;
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

	void print() const {
		printk("Descriptor: %08x %08x", low, high);
	}
} __attribute__ ((packed));


class UserDesc {
private:
	int32_t entry_number;
	uint32_t base_addr;
	uint32_t limit;
	uint32_t flags;
public:
	SegmentDescriptor toGDT() const {
                uint8_t dFlags = 0x00;

                // not system, and dpl 3
                uint8_t dAccess = 0x70;

                if (flags & 0x1) {
			printk("32 bits\n");
			dFlags |= DSC_FLAG_DEFAULT_32;
		}
		if (flags & 0x2) {
			printk("Conforming/Growsdown\n");
			dAccess |= DSC_ACCESS_CONFORMING;
		}
                if (flags & 0x4) {
			printk("Executable\n");
			dAccess |= DSC_ACCESS_EXECUTABLE;
		}
		// INVERTED!!!
                if (! (flags & 0x8)) { // if the flag is 1, the segment is read or executable only!
			printk("Writable/readable\n");
			dAccess |= DSC_ACCESS_WRITABLE;
		}
		if (flags & 0x10) {
			printk("Page granular\n");
			dFlags |= DSC_FLAG_PAGE_GRAN;
		}
		if (! (flags & 0x20)) { // present flag inverted!!!
			printk("Present\n");
			dAccess |= DSC_ACCESS_PRESENT;
		}
		if (flags & 0x40) {
			printk("Extra\n");
			dFlags |= DSC_FLAG_EXTRA;
		}

		SegmentDescriptor rv;
		rv.set_limit(limit, false);
		rv.set_base(base_addr);
		rv.set_flags(dFlags);
		rv.set_access(dAccess);
		rv.print();
		return rv;
	}

	void setEntryNumber(int32_t entry) {
		this->entry_number = entry;
	}

	int32_t getEntryNumber() {
		return entry_number;
	}
} __attribute__((packed));


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

extern SegmentDescriptor GDT[];

#endif
