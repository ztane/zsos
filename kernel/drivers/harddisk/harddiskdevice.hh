#ifndef __HARDDISK_DEVICE_HH__
#define __HARDDISK_DEVICE_HH__

#include "kernel/drivers/block.hh"

class HardDiskDevice : public BlockDevice {
protected:
	uint64_t number_of_sectors;
public:
	HardDiskDevice(uint64_t size);

	virtual FileOffset getSize() {
		return sector_size * number_of_sectors;
	}

        virtual ErrnoCode open (int mode, FileDescriptor*& fd) {
		return EPERM;
	}

        virtual ErrnoCode read (      void *buf, size_t amount, FileOffset offset, size_t& read);
        virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset, size_t& written) {
		return EPERM;
	}

	virtual uint32_t getTotalSectors() const {
		return number_of_sectors;
	}
};


#endif
