// -*- C++ -*-

#ifndef IDE_DISK_INC
#define IDE_DISK__INC 1

#include <iostream>
#include <cstdlib>
#include "ide.hh"
#include "interrupt.hh"
#include "port.h"
#include "blockdevice.hh"

// HardDiskDevice
class IdeHdDev : public BlockDev
{
	public:
		IdeHdDev(int controller, int device);
		virtual ~IdeHdDev();

		size_t read(void *destination, uint32_t block_address, size_t block_count);

		int pushIdeReq(struct IdeReq &req);

	private:
		inline size_t readDataReg(void *dst, size_t bcount);

		// negative values mark a defunct / not in use device
		struct _geometry
		{
			uint32_t cylinders;
			uint32_t heads;
			uint32_t sectors_track;
			uint32_t blocksize;
			// if has LBA then lba := 28 or 48
			uint32_t lba;
			// if has LBA then total_sectors := max_LBA_addressable_sectors
			// FIXME! USE UINT64 HERE
			uint32_t total_sectors;
		} geometry;
		int mailbox;
		int32_t controller, device;
		uint16_t regbase, ctlbase;
};

#endif
