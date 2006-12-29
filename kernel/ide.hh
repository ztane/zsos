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

// IDE requests
struct IdeReq {
	uint32_t pid;		// do we need this?
	void *dst;		// destination address to write to
	uint32_t bAddress;	// block address
	uint32_t bCount;	// block count (dst must have at least bCount * bSize free space)
				// bSize queried from device
	uint32_t ideCtlr, ideDev; // IDE controller id and device id
};

// HardDiskDevice
class IdeHddDev : public BlockDev
{
	public:
		IdeHddDev(int controller, int device);
		virtual ~IdeHddDev();

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
