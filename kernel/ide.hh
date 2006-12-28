// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include "interrupt.hh"
#include "port.h"
#include "blockdevice.hh"

// port addresses for 1st Fixed Disk Controller, Command Block
const unsigned short IDE_REG_BASE	= 0x00;
const unsigned short IDE_DATA 	= 0x00;	// read-write - data register
const unsigned short IDE_ERROR 	= 0x01;	// read-only - error register
const unsigned short IDE_WPC 	= 0x01;	// write-only - Write Precompensation Cylinder divided by 4
const unsigned short IDE_SCNT 	= 0x02;	// read-write - sector count
const unsigned short IDE_SNMBR 	= 0x03;	// read-write - sector number (CHS mode)
const unsigned short IDE_CLOW 	= 0x04;	// read-write - cylinder low (CHS mode)
const unsigned short IDE_CHIGH 	= 0x05;	// read-write - cylinder high (CHS mode)
const unsigned short IDE_DRHD 	= 0x06;	// read-write - drive/head
const unsigned short IDE_STATUS	= 0x07;	// read-only - status register
const unsigned short IDE_CMD	= 0x07;	// write-only - command register

// 1st Fixed Disk Controller, Control Block
const unsigned short IDE_ALT_STATUS	= 0x00;	// read-only
const unsigned short IDE_DEV_CTRL	= 0x00;	// write-only
const unsigned short IDE_DRV_ADDR	= 0x01;	// read-only OBSOLETE

// HardDiskDevice
class IdeHddDev : public BlockDev
{
	public:
		IdeHddDev(int controller, int device);
		virtual ~IdeHddDev();

		size_t read(void *destination, uint32_t block_address, size_t block_count);

	private:
		// negative values mark a defunct / not in use device
		struct _geometry
		{
			unsigned int cylinders;
			unsigned int heads;
			unsigned int sectors_track;
			// if has LBA then total_sectors := max_LBA_addressable_sectors
			unsigned int total_sectors;
			unsigned int blocksize;
			// if has LBA then lba := 28 or 48
			unsigned int lba;
		} geometry;
		int32_t controller, device;
		uint16_t regbase, ctlbase;
};

#endif
