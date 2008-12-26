#ifndef __RAMDIS_DEVICE_HH__
#define __RAMDIS_DEVICE_HH__

#include "kernel/drivers/block.hh"

class RamDiskDevice : public BlockDevice {
private:
	uint32_t size;
	void *buffer;
public:
	RamDiskDevice(void *buffer, uint32_t size);

	virtual FileOffset getSize() {
		return size;
	}

        virtual ErrnoCode open(int mode, FileDescriptor*& fd);
        virtual ErrnoCode read (void *buf, size_t amount, FileOffset offset, size_t& read);
        virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset, size_t& written);
};


#endif
