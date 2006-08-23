// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include "interrupt.hh"
#include "port.h"
#include "blockdevice.hh"

// port addresses for 1st Fixed Disk Controller, Command Block
const unsigned short IDE0_REG_BASE	= 0x01F0;
const unsigned short IDE0_DATA_REG 	= 0x01F0;	// read-write - data register
const unsigned short IDE0_ERROR_REG 	= 0x01F1;	// read-only - error register
const unsigned short IDE0_WPC_REG 	= 0x01F1;	// write-only - Write Precompensation Cylinder divided by 4
const unsigned short IDE0_SCNT_REG 	= 0x01F2;	// read-write - sector count
const unsigned short IDE0_SNMBR_REG 	= 0x01F3;	// read-write - sector number (CHS mode)
const unsigned short IDE0_CLOW_REG 	= 0x01F4;	// read-write - cylinder low (CHS mode)
const unsigned short IDE0_CHIGH_REG 	= 0x01F5;	// read-write - cylinder high (CHS mode)
const unsigned short IDE0_DRHD_REG 	= 0x01F6;	// read-write - drive/head
const unsigned short IDE0_STATUS_REG 	= 0x01F7;	// read-only - status register
const unsigned short IDE0_CMD_REG 	= 0x01F7;	// write-only - command register

// port addresses for 2nd Fixed Disk Controller, Command Block
const unsigned short IDE1_REG_BASE	= 0x0170;
const unsigned short IDE1_DATA_REG 	= 0x0170;	// read-write - data register
const unsigned short IDE1_ERROR_REG 	= 0x0171;	// read-only - error register
const unsigned short IDE1_WPC_REG 	= 0x0171;	// write-only - Write Precompensation Cylinder divided by 4
const unsigned short IDE1_SCNT_REG 	= 0x0172;	// read-write - sector count
const unsigned short IDE1_SNMBR_REG 	= 0x0173;	// read-write - sector number (CHS mode)
const unsigned short IDE1_CLOW_REG 	= 0x0174;	// read-write - cylinder low (CHS mode)
const unsigned short IDE1_CHIGH_REG 	= 0x0175;	// read-write - cylinder high (CHS mode)
const unsigned short IDE1_DRHD_REG 	= 0x0176;	// read-write - drive/head
const unsigned short IDE1_STATUS_REG 	= 0x0177;	// read-only - status register
const unsigned short IDE1_CMD_REG 	= 0x0177;	// write-only - command register

// 1st Fixed Disk Controller, Control Block
const unsigned short IDE0_ALT_STATUS_REG	= 0x03F6;	// read-only
const unsigned short IDE0_DEV_CTRL_REG		= 0x03F6;	// write-only
const unsigned short IDE0_DRV_ADDR_REG		= 0x03F7;	// read-only OBSOLETE

// 2nd Fixed Disk Controller, Control Block
const unsigned short IDE1_ALT_STATUS_REG	= 0x0376;	// read-only
const unsigned short IDE1_DEV_CTRL_REG		= 0x0376;	// write-only
const unsigned short IDE1_DRV_ADDR_REG		= 0x0377;	// read-only OBSOLETE

// HardDiskDevice
class IdeHddDev : public BlockDev
{
	public:
		IdeHddDev(int controller, int device);
		virtual ~IdeHddDev();

		int issueRead(uint32_t block_address, size_t block_count);

	private:
		// negative values mark a defunct / not in use device
		int controller, device;

		// NOTE: these could be implemented with just an base address and
		// some constants addressing the registers:
		// out byte to port reg_base + PORT, data
		struct _registers
		{
			unsigned short data;
			unsigned short error;
			unsigned short wpc;
			unsigned short sec_count;
			unsigned short sec_number;
			unsigned short cyl_low;
			unsigned short cyl_high;
			unsigned short drive_head;
			unsigned short status;
			unsigned short command;
			unsigned short alt_status;
			unsigned short dev_control;
			unsigned short drv_address; // OBSOLETE
		} registers;
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
};

#endif
