#include "block.hh"

ErrnoCode BlockDevice::acquire_sector(uint64_t number, SectorHandle & handle)
{
	uint8_t *buffer = new uint8_t[sector_size];
	size_t bytes_read = 0;
	ErrnoCode rc =
	    read(buffer, sector_size, FileOffset(number * sector_size),
		 bytes_read);

	if (rc != NOERROR || bytes_read != sector_size) {
		delete[] buffer;
	} else {
		handle.init(this, buffer, number);
	}

	return rc;
}
