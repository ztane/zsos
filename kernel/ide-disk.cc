#include <iostream>
#include <cstring>
#include "ide.hh"
#include "printk.h"
#include "interrupt.hh"
#include "port.h"

#define BUFB_LEN 512

IdeHddDev::IdeHddDev(int cntrl, int dev) : 
	controller(cntrl),
	device(dev)
{
	// these are used in Identify Device
	// bufb len is 512, bufw len is 256!
	unsigned char   bufb[BUFB_LEN];
	unsigned short *bufw = (unsigned short *) bufb;

	char serial_number[20+1];
	char firmware_rev [ 8+1];
	char model_number [40+1];

	// don't accept invalid device or controller IDs
	if ((controller | device) & ~0x01) {
		controller = device = -1;
		kout << "IdeHddDev::IdeHddDev: invalid ide or device id. Aborting init." << endl;
		return;
	}

	kout << "IdeHddDev::IdeHddDev: initializing ide" << controller
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
		kout << "ide" << controller << ": error" << endl;
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
		kout << "ide" << controller << ": error" << endl;
		return;
	}

	// if no error read what ever data we got
	for (int i = 0; i < BUFB_LEN / 2; i++)
		bufw[i] = inw(regbase + IDE_DATA);

	// with write(...) these could be done without the memcpy
	// these aren't exactly essential, so we don't save 'em in
	// the object
	memcpy(serial_number, (const char *) &bufw[0x0A], 20); serial_number[20] = '\0';
	memcpy(firmware_rev, (const char *) &bufw[0x17], 8);     firmware_rev[8] = '\0';
	memcpy(model_number, (const char *) &bufw[0x1B], 40);   model_number[40] = '\0';

	geometry.cylinders     = bufw[0x01];
	geometry.heads         = bufw[0x03];
	geometry.sectors_track = bufw[0x06];
	geometry.total_sectors = geometry.cylinders * geometry.heads * geometry.sectors_track;

	// LBA
	

	kout << "\tmodel: " << model_number 
		<< " fw rev: " << firmware_rev << endl;
	kout << "\tserial: " << serial_number << endl;
	kout << "\tC,H,S/T: " << geometry.cylinders
		<< ", " << geometry.heads << ", " << geometry.sectors_track
		<< " total sectors: " << geometry.total_sectors << endl;
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
		kout << "IdeHddDev::issueRead: error uninitialized driver" << endl;
		return 0;
	}

	// reading 0 blocks is.. illogical
	if (bcount < 1) {
		kout << "IdeHddDev::issueRead: error attempt to read 0 blocks" << endl;
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
	uint8_t sector    = (baddr % geometry.sectors_track) + 1;
	uint8_t head      = (baddr / geometry.sectors_track) % geometry.heads;
	uint16_t cylinder = (baddr / geometry.sectors_track) / geometry.heads;

        // let's get started! wait till the device is ready...
        // 0x50 : (bit 6) drive is ready, (bit 4) seek complete
        // 0x01 : (bit 0) previous command ended in an error
        while (! (inb(regbase + IDE_STATUS) & (0x50 | 0x01)))
                io_wait();
        if (inb(regbase + IDE_STATUS) & 0x01) {
                kout << "ide" << controller << ": error" << endl;
                return 0;
        }

	// load command registers
        // select correct drive, set head
	// FIXME! datatypes.
        outb(regbase + IDE_DRHD, (0x0A | device) << 4 | head);
	outb(regbase + IDE_CLOW, cylinder);
	outb(regbase + IDE_CHIGH, cylinder >> 8);
	outb(regbase + IDE_SNMBR, sector);
	outb(regbase + IDE_SCNT, bcount);

        // 0x50 : (bit 6) drive is ready, (bit 4) seek complete
        // 0x01 : (bit 0) previous command ended in an error
        while (! (inb(regbase + IDE_STATUS) & (0x50 | 0x01)))
                io_wait();
        if (inb(regbase + IDE_STATUS) & 0x01) {
                kout << "ide" << controller << ": error" << endl;
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
                kout << "ide" << controller << ": error" << endl;
                return 0;
        }

	uint16_t *buf = (uint16_t *) dst;
        // if no error read what ever data we got
	while (bcount --)
	        for (int i = 0; i < 256; i++)
			*(buf + i) = inw(regbase + IDE_DATA);

	return 0;
}
