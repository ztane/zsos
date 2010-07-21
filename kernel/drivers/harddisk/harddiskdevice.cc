#include "kernel/drivers/harddisk/harddiskdevice.hh"
#include "kernel/simpleide.hh"
#include <string.h>
#include <iostream>

HardDiskDevice::HardDiskDevice(uint64_t size) {
	sector_size = 512;
	this->number_of_sectors = uint32_t(size) / sector_size;
}

ErrnoCode HardDiskDevice::read(void *buf, size_t amount, FileOffset off, size_t& read) {
	if (amount % sector_size) {
		return EINVAL;
	}

	if ((uint32_t)off.to_scalar() % sector_size) {
		return EINVAL;
	}

	uint64_t start_sector = uint32_t(off.to_scalar()) / sector_size;
	uint64_t num_sectors  = amount / sector_size;

	if (start_sector >= number_of_sectors) {
		read = 0;
		return NOERROR;
	}

	size_t sectors_remaining = number_of_sectors - start_sector;
	if (sectors_remaining < num_sectors) {
		num_sectors = sectors_remaining;
	}

	do_read_ide(start_sector, num_sectors, buf);
	read = num_sectors * sector_size;

	return NOERROR;
}
