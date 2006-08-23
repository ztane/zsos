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

	// do NOT accept invalid device or controller IDs
	if ((controller | device) > 1)
	{
		controller = device = -1;
		kout << "IdeHddDev::IdeHddDev: invalid ide or device id. Aborting init." << endl;
		return;
	}

	kout << "IdeHddDev::IdeHddDev: initializing ide" << controller
		<< ", device" << device << endl;

	// set up correct Command Block registers:
	// controller 0 base is 0x01F0 and 1 is 0x0170
	unsigned short reg_base = IDE0_REG_BASE - controller * 0x80;

	registers.data       = reg_base;
	registers.error      = reg_base + 0x01;
	registers.wpc        = reg_base + 0x01;
	registers.sec_count  = reg_base + 0x02;
	registers.sec_number = reg_base + 0x03;
	registers.cyl_low    = reg_base + 0x04;
	registers.cyl_high   = reg_base + 0x05;
	registers.drive_head = reg_base + 0x06;
	registers.status     = reg_base + 0x07;
	registers.command    = reg_base + 0x07;

	// set up correct Control Block registers:
	// controller 0 base is 0x03F6 and 1 is 0x0376
	reg_base += 0x0206;

	registers.alt_status  = reg_base;
	registers.dev_control = reg_base;
	registers.drv_address = reg_base + 1;

	// FIXME: INTERRUPTS

	// select correct drive
	outb(registers.drive_head, (0x0A | device) << 4);

	// let's get started! wait till the device is ready...
	// 0x50 : (bit 6) drive is ready, (bit 4) seek complete
	// 0x01 : (bit 0) previous command ended in an error
	while (! (inb(registers.status) & (0x50 | 0x01)))
		io_wait();
	if (inb(registers.status) & 0x01)
	{
		kout << "ide" << controller << ": error" << endl;
		return;
	}

	// drive ready! issue Identify Drive (0xEC) command
	outb(registers.command, 0xEC);

	// wait till we have data in buffer
	// 0x58 : (bit 6) drive is ready, (bit 4) seek complete,
	//        (bit 3) sector buffer requires servicing
	// 0x01 : (bit 0) previous command ended in an error
	while (! (inb(registers.status) & (0x58 | 0x01)))
		io_wait();
	// error handling here!
	if (inb(registers.status) & 0x01)
	{
		kout << "ide" << controller << ": error" << endl;
		return;
	}

	// if no error read what ever data we got
	for (int i = 0; i < BUFB_LEN / 2; i++)
		bufw[i] = inw(registers.data);

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

	// FIXME: !!!
	// I'M QUITE SHURE THIS SHOULDN'T BE DONE HERE
	// enable IRQ 14
	// FOR TESTING PURPOSES ONLY
	outb(0x21, inb(0x21) & 0xFB);
	io_wait();
	outb(0xA1, inb(0xA1) & 0xBF);
	io_wait();
}

IdeHddDev::~IdeHddDev()
{

}

int IdeHddDev::issueRead(uint32_t baddr, size_t bcount)
{
	// no mind in doing anything if driver has not
	// been initialized correctly
	if (device < 0)
	{
		kout << "IdeHddDev::issueRead: error uninitialized driver" << endl;
		return -1;
	}

	// reading 0 blocks is.. illogical
	if (bcount < 1)
	{
		kout << "IdeHddDev::issueRead: error attempt to read 0 blocks" << endl;
		return -1;
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
	int sector   = (baddr % geometry.sectors_track) + 1;
	int head     = (baddr / geometry.sectors_track) % geometry.heads;
	int cylinder = (baddr / geometry.sectors_track) / geometry.heads;

	// inform others that we are doing serious stuff
	enter_critical();

	// select correct drive
	outb(registers.drive_head, (0x0A | device) << 4);

	// 1. Wait for drive to clear BUSY.
	while (inb(registers.status) & 0x80)
		io_wait();

	// NOTE: CHS method
	// 2. Load required parameters in the Command Block Registers.
	outb(registers.drive_head, ((0x0A | device) << 4) | (head & 0x0F));
	outb(registers.cyl_low, cylinder);
	outb(registers.cyl_high, cylinder >> 8);
	outb(registers.sec_number, sector);
	outb(registers.sec_count, bcount);

	// 4. Wait for drive to set DRDY.
	while (! (inb(registers.status) & 0x40))
		io_wait();

	// 5. Write the command code to the Command Register.
	// 0x20: Read sectors with retry
	outb(registers.command, 0x20);

	// we are good to go
	leave_critical();

	// 0 means EXIT_SUCCESS
	return 0;
}
