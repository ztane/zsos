#include <iostream>
#include <cstdlib>
#include <cstring>
#include "../sim/mbox.h"
#include "ide.hh"
#include "printk.h"
#include "interrupt.hh"
#include "port.h"

IdeHddDev::IdeHddDev(int cntrl, int dev) : 
	controller(cntrl),
	device(dev)
{
	// these are used in Identify Device
	uint16_t bufw[256]; // data register is 16bit wide, 256 words
	char serial_number[20+1];
	char firmware_rev [ 8+1];
	char model_number [40+1];

	// don't accept invalid device or controller IDs
	if ((controller | device) & ~0x01) {
		controller = device = -1;
		kout << __FUNCTION__ << ": invalid ide or device id. Aborting init." << endl;
		return;
	}

	kout << __FUNCTION__ << ": initializing ide" << controller
		<< ", device" << device << endl;

	// set up correct Command Block registers:
	// controller 0 base is 0x01F0 and 1 is 0x0170
	regbase = 0x01F0 - controller * 0x80;

	// set up correct Control Block registers:
	// controller 0 base is 0x03F6 and 1 is 0x0376
	ctlbase = regbase + 0x0206;

	// FIXME: INTERRUPTS

	// select correct drive
	outb(regbase + IDE_DRHD, (0x0A | device) << 4);

	// let's get started! wait till the device is ready...
	// 0x50 : (bit 6) drive is ready, (bit 4) seek complete
	// 0x01 : (bit 0) previous command ended in an error
	while (! (inb(regbase + IDE_STATUS) & (0x50 | 0x01)))
		io_wait();
	if (inb(regbase + IDE_STATUS) & 0x01) {
		controller = device = -1;
		kout << __FUNCTION__ << ": ide" << controller << ": error" << endl;
		return;
	}

	// drive ready! issue Identify Drive (0xEC) command
	outb(regbase + IDE_CMD, 0xEC);

	// wait till we have data in buffer
	// 0x58 : (bit 6) drive is ready, (bit 4) seek complete,
	//        (bit 3) sector buffer requires servicing
	// 0x01 : (bit 0) previous command ended in an error
	while (! (inb(regbase + IDE_STATUS) & (0x58 | 0x01)))
		io_wait();
	// error handling here!
	if (inb(regbase + IDE_STATUS) & 0x01) {
		controller = device = -1;
		kout << __FUNCTION__ << ": ide" << controller << ": error" << endl;
		return;
	}

	// if no error read what ever data we got
	readDataReg(bufw, 1);

	// with write(...) these could be done without the memcpy
	// these aren't exactly essential, so we don't save 'em in
	// the object
	memcpy(serial_number, (const char *) &bufw[0x0A], 20); serial_number[20] = '\0';
	memcpy(firmware_rev, (const char *) &bufw[0x17], 8);     firmware_rev[8] = '\0';
	memcpy(model_number, (const char *) &bufw[0x1B], 40);   model_number[40] = '\0';

	geometry.cylinders     = bufw[0x01];
	geometry.heads         = bufw[0x03];
	geometry.sectors_track = bufw[0x06];

	// Does the device support LBA
	// word #49 bit 9 is set if device has LBA
	geometry.total_sectors = 0;
	if (bufw[0x31] & 0x0200) {
		// LBA 48? (word #83 bit 10 is set)
		if (bufw[0x53] & 0x0400)
			geometry.lba = 48;
		else
			geometry.lba = 28;
	} else {
		geometry.lba = 0;
	}
	geometry.total_sectors = bufw[0x3C] | bufw[0x3D] << 16;
	
	kout << "\tmodel: " << model_number << " fw rev: " << firmware_rev << endl;
	kout << "\tserial: " << serial_number << endl;
	kout << "\tC,H,S/T: " << geometry.cylinders << ", " << geometry.heads << ", " << geometry.sectors_track;
	if (geometry.lba)
		kout << " LBA: " << geometry.lba;
	kout << " total sectors: " << geometry.total_sectors << endl;
}

IdeHddDev::~IdeHddDev()
{

}

/**
 * FIXME! PIO read, doesn't even use IRQ!
 */
size_t IdeHddDev::read(void *dst, uint32_t baddr, size_t bcount)
{
	// no mind in doing anything if driver has not
	// been initialized correctly
	if (device < 0) {
		kout << __FUNCTION__ << ": error uninitialized driver" << endl;
		return 0;
	}

	// reading 0 blocks is.. illogical
	if (bcount < 1) {
		kout << __FUNCTION__ << ": error attempt to read 0 blocks" << endl;
		return 0;
	}

//  1. Wait for drive to clear BUSY.
//  2. Load required parameters in the Command Block Registers.
//  3. Activate the Interrupt Enable (nIEN) bit.
//  4. Wait for drive to set DRDY.
//  5. Write the command code to the Command Register.

// LBA -> CHS:
// sector   = (LBA mod SectorsPerTrack) + 1
// head     = (LBA / SectorsPerTrack) mod NumberOfHeads
// cylinder = (LBA / SectorsPerTrack) / NumberOfHeads
	uint8_t sector = 0, head = 0;
	uint16_t cylinder = 0;

	if (!geometry.lba) {
		sector   = (baddr % geometry.sectors_track) + 1;
		head     = (baddr / geometry.sectors_track) % geometry.heads;
		cylinder = (baddr / geometry.sectors_track) / geometry.heads;
	}
        // let's get started! wait till the device is ready...
        // 0x50 : (bit 6) drive is ready, (bit 4) seek complete
        // 0x01 : (bit 0) previous command ended in an error
        while (! (inb(regbase + IDE_STATUS) & (0x50 | 0x01)))
                io_wait();
        if (inb(regbase + IDE_STATUS) & 0x01) {
		kout << __FUNCTION__ << ": ide" << controller << ": error" << endl;
                return 0;
        }

	switch (geometry.lba) {
	case 48:
		kout << __FUNCTION__ << ": no LBA48 support yet, sorry." << endl;
		return 0;
	case 28:
		outb(regbase + IDE_DRHD, (0x0E | device) << 4 | ((baddr >> 24) & 0x0f));
		outb(regbase + IDE_CHIGH, baddr >> 16);
		outb(regbase + IDE_CLOW, baddr >> 8);
		outb(regbase + IDE_SNMBR, baddr);
		break;
	case 0:
	        outb(regbase + IDE_DRHD, (0x0A | device) << 4 | head);
		outb(regbase + IDE_CLOW, cylinder);
		outb(regbase + IDE_CHIGH, cylinder >> 8);
		outb(regbase + IDE_SNMBR, sector);
		break;
	default:
		break;
	}
	outb(regbase + IDE_SCNT, bcount);

        // 0x50 : (bit 6) drive is ready, (bit 4) seek complete
        // 0x01 : (bit 0) previous command ended in an error
        while (! (inb(regbase + IDE_STATUS) & (0x50 | 0x01)))
                io_wait();
        if (inb(regbase + IDE_STATUS) & 0x01) {
		kout << __FUNCTION__ << ": ide" << controller << ": error" << endl;
                return 0;
        }

        // drive ready! issue Read Sectors (0x21) command
        outb(regbase + IDE_CMD, 0x21);

        // wait till we have data in buffer
        // 0x58 : (bit 6) drive is ready, (bit 4) seek complete,
        //        (bit 3) sector buffer requires servicing
        // 0x01 : (bit 0) previous command ended in an error
        while (! (inb(regbase + IDE_STATUS) & (0x58 | 0x01)))
                io_wait();
        // error handling here!
        if (inb(regbase + IDE_STATUS) & 0x01) {
		kout << __FUNCTION__ << ": ide" << controller << ": error" << endl;
                return 0;
        }

        // if no error read what ever data we got
	readDataReg(dst, bcount);

	printk("\n");
	return 0;
}

/*
 * Buffered (IRQ handled) requests should call this to add request to que
 */
int IdeHddDev::pushIdeReq(struct IdeReq &request)
{
	// PROCESS:
	/*
		reading process:
		if IdeRequest que empty (device unlocked)
			push request to que
			send request to device (since IRQ doesn't launch without...)
		else
			push request to que

	*/
	return 0;
}

inline size_t IdeHddDev::readDataReg(void *dst, size_t bcount)
{
	// NOTE: IDE data register is 16bit wide and
	// sector buffer holds 512 bytes
	uint16_t *buf = (uint16_t *) dst;
        uint32_t wordcount = bcount * (512 / 2);

        for (uint32_t i = 0; i < wordcount; i++) {
                *(buf + i) = inw(regbase + IDE_DATA);
/*
		if (!(i % 16))
			kout << endl;
		printk("%02x", *((uint8_t *) (buf + i)));
		printk("%02x", *((uint8_t *) (buf + i) + 1));
*/
	}
	return bcount;
}
