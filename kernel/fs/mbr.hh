#ifndef __MBR__HH__
#define __MBR__HH__

#include <stddef.h>
#include <stdint.h>
#include "hexdump.hh"
#include <iostream>

class PartitionRecord {
private:
	uint32_t start_chs;
	uint32_t end_chs;
	uint32_t start_lba;
	uint32_t size_lba;
	uint8_t  type;
	uint8_t  flags;
public:
	void initialize(const uint8_t* data) {
		flags = data[0];
		type  = data[4];

		start_lba = (uint32_t)data[8] +
			((uint32_t)data[9] << 8) +
			((uint32_t)data[10] << 16) +
			((uint32_t)data[11] << 24);

		size_lba = (uint32_t)data[12] +
			((uint32_t)data[13] << 8) +
			((uint32_t)data[14] << 16) +
			((uint32_t)data[15] << 24);
	}

	bool isActive() const {
		return flags & 0x80;
	}

	uint8_t getType() const {
		return type;
	}

	uint32_t getStart() const {
		return start_lba;
	}

	uint32_t getSize() const {
		return size_lba;
	}

	uint32_t getEnd() const {
		return start_lba + size_lba - 1;
	}

	bool isValid() const {
		return type && size_lba && start_lba;
	}
};

class MBR {
private:
	PartitionRecord records[4];
public:
	void initialize(const void *boot_sector) {
		const char *dataptr = (const char*)boot_sector;
		dataptr += 446;
		for (int i = 0; i < 4; i++) {
			records[i].initialize((const uint8_t *)dataptr);
			dataptr += 16;
		}
	}

	const PartitionRecord *getPartitionRecord(uint32_t number) const {
		if (number < 4) {
			if (records[number].isValid()) {
				return &records[number];
			}
		}

		return NULL;
	}
};

#endif
