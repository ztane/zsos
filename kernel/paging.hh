// -*- C++ -*-

#ifndef PAGING_INCLUDED
#define PAGING_INCLUDED

class PageFlags {
protected:
	unsigned long flags;
public:
	PageFlags() { flags = 0; }
	
	unsigned long get_flags() {
		return flags;
	}

	void set_flags(unsigned long flags) {
		this->flags = flags;
	}
	
	void set_present(bool present) {
		flags &= ~ 1;
		flags |= present ? 1 : 0;
	}

	void set_rw(bool present) {
		flags &= ~ 2;
		flags |= present ? 2 : 0;
	}

	void set_4MiB(bool is) {
		flags &= ~ 0x80;
		flags |=      is ? 0x80 : 0;
	}
	
	bool is_present() const {
		return flags & 1;
	}

	bool is_rw() const {
		return flags & 2;
	}

	bool is_4MiB() const {
		return flags & 0x80;
	}
};

class PageTableEntry {
protected:
	unsigned long data;
public:
	PageTableEntry() {	data = 0; }
	PageTableEntry(PageFlags flags, void *base_addr) {
		data = (unsigned long)base_addr & ~ 0xFFFFF000;
		data |= flags.get_flags();
	}

	PageFlags get_flags() {
		PageFlags flags;
		flags.set_flags(data & 0xFFF);
		return flags;
	}

	void set_flags(PageFlags flags) {
		data &= ~ 0xFFF;
		data |= flags.get_flags();
	}

	void* get_offset() {
		return (void*)(data & ~0xFFF);
	}

	void set_offset(unsigned long offset) {
		data &= ~ 0xFFFFF000;
		data |= offset & ~0xFFF;
	}

	void set_offset(void *offset) {
		data &= ~ 0xFFFFF000;
		data |= (unsigned long)offset & ~0xFFF;
	}

	void clear() {
		data = 0;
	}
};

void initialize_page_tables();
void disable_null_page();

#endif