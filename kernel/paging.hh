// -*- C++ -*-

#ifndef PAGING_INCLUDED
#define PAGING_INCLUDED

#include "kernel/arch/current/virtaddr.hh"
#include "kernel/mm/pageframe.hh"
#include <string.h>

struct PageFaultInfo {
	VirtAddr address;

	bool userMode;
	bool write;
	bool present;
};

class PageFlags {
protected:
	unsigned long flags;
public:
	PageFlags() { flags = 0; }
	
	unsigned long getFlags() {
		return flags;
	}

	void setFlags(unsigned long flags) {
		this->flags = flags;
	}
	
	void setPresent(bool present) {
		flags &= ~ 1;
		flags |= present ? 1 : 0;
	}

	void setRW(bool present) {
		flags &= ~ 2;
		flags |= present ? 2 : 0;
	}

	void setUser(bool is) {
		flags &= ~ 4;
		flags |= is ? 4 : 0;
	}

	void setHuge(bool is) {
		flags &= ~ 0x80;
		flags |=      is ? 0x80 : 0;
	}
	
	bool isPresent() const {
		return flags & 1;
	}

	bool isRW() const {
		return flags & 2;
	}

	bool isUser() const {
		return flags & 0x4;
	}

	bool isHuge() const {
		return flags & 0x80;
	}
};

class CommonPDE {
protected:
	unsigned long data;

public:
	inline PageFlags getFlags() {
		PageFlags flags;
		flags.setFlags(data & 0xFFF);
		return flags;
	}

	inline void setFlags(PageFlags flags) {
		data &= ~ 0xFFF;
		data |= flags.getFlags();
	}

	inline pageaddr_t getPageAddr() {
		return data >> 12;
	}

	inline void* getVirtAddr() {
		return getPageAddr().toVirtual();
	}

	inline void setPageAddr(pageaddr_t a) {
		data &= ~ 0xFFFFF000;
		data |= (a << 12);
	}

	inline void setVirtAddr(void *offset) {
		setPageAddr(pageaddr_t::fromVirtual(offset));
	}

	inline void clear() {
		data = 0;
	}
};

class PageTable;

class PageDirectoryEntry : public CommonPDE {
public:
	PageDirectoryEntry() {	data = 0; }
	PageDirectoryEntry(PageFlags flags, void *base_addr) {
		data = (unsigned long)base_addr & ~ 0xFFFFF000;
		data |= flags.getFlags();
	}

	PageTable *getPageTable() {
		if ((! getFlags().isPresent()) ||
			getFlags().isHuge())
				// sorry, no page table here ;)
				return 0;

		return (PageTable*)getVirtAddr();
	}
};

class PageTableEntry : public CommonPDE {
public:
	PageTableEntry() {	data = 0; }
	PageTableEntry(PageFlags flags, void *base_addr) {
		data = (unsigned long)base_addr & ~ 0xFFFFF000;
		data |= flags.getFlags();
	}
};

class PageTable {
protected:
	PageTableEntry entries[1024];
public:
	void clear() {
		memset(entries, 0, sizeof entries);
	}

	inline PageTableEntry *getPTE(VirtAddr a) {
		uint32_t t = (a >> 12) & 0x3FF;
		return entries + t;
	}
};

class PageDirectory {
protected:
	PageDirectoryEntry entries[1024];
public:
	void clear() {
		memset(entries, 0, sizeof entries);
	}

	inline PageDirectoryEntry *getPDE(VirtAddr a) {
		uint32_t t = a >> 22;
		return entries + t;
	}

	inline PageTableEntry *getPTE(VirtAddr a) {
		PageDirectoryEntry *pde = getPDE(a);
		PageTable *t = pde->getPageTable();

		if (! t)
			return 0;

		return t->getPTE(a);
	}
};

void initialize_page_tables();
void disable_null_page();

extern PageDirectory  *page_directory;
uint32_t mapPage(PageDirectory *d, VirtAddr a, PageFrame *f, PageFlags fl);

#endif
