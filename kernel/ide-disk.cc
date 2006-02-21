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
							// 	- Logical Bloack Address, bits 7-0 (LBA mode)
const unsigned short IDE1_CLOW_REG 	= 0x01F4;	// read-write - cylinder low (CHS mode)
							// 	- Logical Bloack Address, bits 15-8 (LBA mode)
const unsigned short IDE1_CHIGH_REG 	= 0x01F5;	// read-write - cylinder high (CHS mode)
							// 	- Logical Bloack Address, bits 23-16 (LBA mode)
const unsigned short IDE1_DRHD_REG 	= 0x01F6;	// read-write - drive/head
const unsigned short IDE1_STATUS_REG 	= 0x01F7;	// read-only - status register
const unsigned short IDE1_CMD_REG 	= 0x01F7;	// write-only - command register

HddDev::HddDev()
{
	if ((inb(IDE1_STATUS_REG) & 0x40))
		kout << "IDE1: drive ready" << endl;
	bufw = (unsigned short *) buf;
	read();
}

HddDev::~HddDev()
{

}

void HddDev::read()
// At The Moment read only the first sector of the 1st disk
// connected to 1st IDE controller
{
	// poll the drive state, 0x40 = drive ready
	// remember kids, this BLOCKS!
	while (! (inb(IDE1_STATUS_REG) & 0x40)) {}

	drive      = 0;
	cylinder   = 0;
	head       = 1;
	sector     = 1;
	sctr_count = 1;

	outb(IDE1_SCNT_REG, sctr_count);
	outb(IDE1_SNMBR_REG, sector);
	outb(IDE1_CLOW_REG, cylinder);
	outb(IDE1_CHIGH_REG, (cylinder >> 8));
	outb(IDE1_DRHD_REG, (0xA0 | (drive << 4) | (head & 0x0F)));

	// read sectors with retry
	outb(IDE1_CMD_REG, 0x20);

	// wait until data in buffer
	while (! (inb(IDE1_STATUS_REG) & 0x08)) {}

	kout << "IDE1: first sector: " << endl;

	for (int i = 0; i < 256; i++)
		bufw[i] = inw(IDE1_DATA_REG);

	if (! inb(IDE1_ERROR_REG))
		for (int i = 0; i < 512; i++)
			printk("%x", buf[i]);

	kout << endl;

	printk("IDE1: error=0x%x, status=0x%x\n",
	inb(IDE1_ERROR_REG), inb(IDE1_STATUS_REG));
}
