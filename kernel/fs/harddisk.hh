#ifndef HARDDISK_HH_INCLUDED
#define HARDDISK_HH_INCLUDED

#include "string.h"
#include "kernel/drivers/block.hh"
#include "kernel/fs/mbr.hh"
#include "kernel/errnocode.hh"

class HardDisk;

class Partition : public BlockDevice {
private:
	FileOffset offset;
	FileOffset size;
	HardDisk *container;
	FileOffset end;
	uint64_t   num_sectors;
public:
	Partition(FileOffset offset, FileOffset size, HardDisk *container) : 
		offset(offset), size(size), container(container),
		end(offset + size - 1) {
		sector_size = 512;
		num_sectors = (size.to_scalar() + sector_size - 1)/ sector_size;
	}

        virtual bool isDir() {
		return false;
	}

        virtual FileOffset getSize() const {
		return size;
	};

	virtual uint32_t getTotalSectors() const {
		return num_sectors;
	}

        virtual ErrnoCode open(int mode, FileDescriptor*& fd) {
		// for now...
		return EPERM;
	}

        virtual ErrnoCode read (void *buf, size_t amount, FileOffset offset, size_t& read);

        virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset, size_t& written) {
		// for now;
		return EPERM;
	}

        virtual int getInode() {
                return 0;
        }
};

#define MAX_PARTITIONS 16
class MBR;

class HardDisk {
private:
	FileLike *device;
	MBR *mbr;
	Partition *partitions[MAX_PARTITIONS];
	uint32_t num_partitions;
public:
	HardDisk(FileLike &device) {
		this->device = &device;
		this->mbr = new MBR();
	}

	ErrnoCode initialize() {
		char *sector_buffer = new char[512];
		size_t read;
		ErrnoCode rc = this->device->read(sector_buffer, 512, FileOffset(0), read);
		if (read != 512) {
			rc = EIO;
		}
		if (rc) {
			return rc;
		}

		mbr->initialize(sector_buffer);
		delete[] sector_buffer;

		memset(partitions, 0, sizeof(partitions));
		int i = 0;
		num_partitions = 0;
		for (int i = 0; i < 4; i++) {
			const PartitionRecord *rec = mbr->getPartitionRecord(i);
			if (rec) {
				FileOffset start = (uint64_t)rec->getStart() * 512;
				FileOffset size  = (uint64_t)rec->getSize()  * 512;
				partitions[num_partitions] = new Partition(start, size, this);
				num_partitions ++;
			}
		}
		return NOERROR;
	}

	FileLike *getDevice() const {
		return device;
	}

	Partition *getPartition(uint32_t number) {
		if (number >= MAX_PARTITIONS)
			return NULL;

		return partitions[number];
	}

	uint32_t getNumPartitions() const {
		return num_partitions;
	}

	MBR *getMbr() const {
		return mbr;
	}
};

#endif
