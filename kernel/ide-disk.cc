#include <iostream>
#include "ide.hh"
#include "printk.h"
#include "interrupt.h"
#include "port.h"

// port addresses for 1st Fixed Disk Controller
const unsigned short IDE1_DATA_REG 	= 0x01F0;	// read-write - data register
const unsigned short IDE1_ERROR_REG 	= 0x01F1;	// read-only - error register
const unsigned short IDE1_WPC_REG 	= 0x01F1;	// write-only - Write Precompensation Cylinder divided by 4
const unsigned short IDE1_SCNT_REG 	= 0x01F2;	// read-write - sector count
const unsigned short IDE1_SNMBR_REG 	= 0x01F3;	// read-write - sector number (CHS mode)
							// 	- Logical Block Address, bits 7-0 (LBA mode)
const unsigned short IDE1_CLOW_REG 	= 0x01F4;	// read-write - cylinder low (CHS mode)
							// 	- Logical Block Address, bits 15-8 (LBA mode)
const unsigned short IDE1_CHIGH_REG 	= 0x01F5;	// read-write - cylinder high (CHS mode)
							// 	- Logical Block Address, bits 23-16 (LBA mode)
const unsigned short IDE1_DRHD_REG 	= 0x01F6;	// read-write - drive/head
							// 	- Head portion contains Logical Block Address, bits 27-24 (LBA mode)
							// 	- bit 6 controls LBA: 0 off / 1 on
							// 	- bits 7 and 5 are ALWAYS 1
const unsigned short IDE1_STATUS_REG 	= 0x01F7;	// read-only - status register
const unsigned short IDE1_CMD_REG 	= 0x01F7;	// write-only - command register

HddDev::HddDev()
{
	bufw = (unsigned short *) buf;
	drive      = 0;
	cylinder   = 0;
	head       = 0;
	sector     = 0;
	sctr_count = 0;

	while (inb(IDE1_STATUS_REG) != 0x50)
		;
	// drive 0 -> 0xA0 (bit 4 determines drive)
	// ugly hardcoding
	outb(IDE1_DRHD_REG, 0xA0);
	// issue command: Identify Drive
	// this PRESUMES that there is a HDD in 1st contr, 1st device
	outb(IDE1_CMD_REG, 0xEC);
	// wait until data ready
	while (inb(IDE1_STATUS_REG) != 0x58)
		;
	// read data
	for (int i = 0; i < 256; i++)
		bufw[i] = inw(IDE1_DATA_REG);

	geometry.cylinders     = bufw[0x01];
	geometry.heads         = bufw[0x03];
	geometry.block_size    = bufw[0x05];
	geometry.sectors_track = bufw[0x06];
	geometry.total_sectors = geometry.cylinders * geometry.heads * geometry.sectors_track;

	kout << "IDE1: C,H,S/T: " << geometry.cylinders << ", " << geometry.heads
		<< ", " << geometry.sectors_track << endl;
	kout << "IDE1: block size: " << geometry.block_size << endl;
	kout << "IDE1: total sectors: " << geometry.total_sectors << endl;

	read(NULL, 0, 1);
}

HddDev::~HddDev()
{

}

// FIXME: addr should be 64 bit integer -- int64 or so

size_t HddDev::read(void *vbuf, int baddr, size_t bcount)
// At The Moment read only the first sector of the 1st disk
// connected to 1st IDE controller
{
	// poll the drive state, 0x50 = drive ready
	// remember kids, this BLOCKS!
	while (inb(IDE1_STATUS_REG) != 0x50)
		;

	// for now: translate logical address to CHS
	sector     = (baddr % geometry.total_sectors) + 1;
	head       = ((baddr / geometry.total_sectors) % (geometry.heads + 1));
	cylinder   = ((baddr / geometry.total_sectors) / (geometry.heads + 1));
	sctr_count = bcount;

	outb(IDE1_SCNT_REG, sctr_count);
	outb(IDE1_SNMBR_REG, sector);
	outb(IDE1_CLOW_REG, cylinder);
	outb(IDE1_CHIGH_REG, (cylinder >> 8));
	outb(IDE1_DRHD_REG, (0xA0 | (drive << 4) | (head & 0x0F)));

	// read sectors with retry
	outb(IDE1_CMD_REG, 0x20);

	// wait until data in buffer
	while (inb(IDE1_STATUS_REG) != 0x58)
		;

	kout << "IDE1: first sector: " << endl;

	for (int i = 0; i < 256; i++)
		bufw[i] = inw(IDE1_DATA_REG);

	for (int i = 0; i < 512; i++)
		printk("%02x", buf[i]);

	kout << endl;

	printk("IDE1: error=0x%x, status=0x%x\n",
	inb(IDE1_ERROR_REG), inb(IDE1_STATUS_REG));

	return 0;
}
