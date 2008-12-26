#include "kernel/drivers/ramdisk/ramdiskdevice.hh"
#include <string.h>
#include <iostream>

RamDiskDevice::RamDiskDevice(void *buffer, uint32_t size) {
	this->buffer = buffer;
	this->size   = size;
}

ErrnoCode RamDiskDevice::open(int mode, FileDescriptor*& fd) {
        return EPERM;
}

ErrnoCode RamDiskDevice::read(void *buf, size_t amount, FileOffset off, size_t& read) {
	if (off >= size) {
		read = 0;
		return NOERROR;
	}

	size_t bytes_remaining = size_t((FileOffset(size) - off).to_scalar());
	if (bytes_remaining < amount) {
		amount = bytes_remaining;
	}

	read = amount;
	memcpy(buf, (char*)buffer + off.to_scalar(), amount);
	return NOERROR;
}

ErrnoCode RamDiskDevice::write(const void *buf, size_t amount, FileOffset off, size_t& written) {
	return EINVAL;
}
