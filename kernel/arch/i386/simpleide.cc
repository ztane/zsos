#include "kernel/arch/current/port.h"
#include <inttypes.h>
#include <stddef.h>
#include <iostream>
#include "kernel/panic.hh"
#include <printk.h>

#define DCR  0x3F6
#define NCR  0x1F7

// fixed software reset for hda
void reset_ide_disk() {
        outb(DCR, 6);
        inb(DCR);
        inb(DCR);
        inb(DCR);
        inb(DCR);
        outb(DCR, 2);
        inb(DCR);
        inb(DCR);
        inb(DCR);
        inb(DCR);

        for (int ctr = 0; ctr < 10000; ctr++) {
                 uint8_t s = inb(DCR);
                 if ((s & 0xC4) == 0x40) {
                         return;
	         }
	}
        kernelPanic("HDD RESET FAILED");
}

#define SECTOR_SIZE 512

#define MAX_READ (SECTOR_SIZE * 256)

static void _do_read_sector(uint64_t start, uint64_t number, char *dataptr) {
	// acknowledges interrupts....
	uint8_t status;

	// command issued...
	inb(DCR);
	inb(DCR);
	inb(DCR);
	inb(DCR);

	while (true) {
		status = inb(DCR);
		if (status & 0x80) {
			continue; // busy
		}

		if (status & 0x21) {
			kout << (uint32_t)start << " " << (uint32_t)number << " " << (void*)dataptr << endl;
			kernelPanic("IDE ERROR :-)");
		}

		if (status & 0x8) {
			break;
		}
	}

	for (number = 256; number > 0; number --) {
		*((uint16_t*)dataptr) = inw(0x1F0);
		dataptr += 2;
	}
}


// issue 1 read command, and read the results using pio
static void _do_read(uint64_t start, uint32_t number, void *data) {
	char *dataptr = (char*)data;
	uint8_t stat = inb(DCR);
        if (stat & 0x88) {
		reset_ide_disk();
	}

	// MUST WORK HERE... (panic)

	// lets do the trick... using 28-bit lba
        // select the master drive (slave = 0xF0)
	uint8_t select_and_top = 0xE0 | ((start >> 24) & 0xF);
	outb(0x1F6, select_and_top);

	outb(0x1F2, number & 0xFF);

	// push 24 bit of lba
	outb(0x1F3, start);
	outb(0x1F4, start >> 8);
	outb(0x1F5, start >> 16);

	// commence read PIO 28
	outb(0x1F7, 0x20);

	if (number == 0) {
		number = 256;
	}

	for ( ; number > 0; number --) {
		_do_read_sector(start, number, dataptr);
		dataptr += SECTOR_SIZE;
	}
}

void do_read_ide(uint64_t start, uint32_t number, void *data) {
	char *dataptr = (char*)data;
	uint32_t full_counts = number / 256;
	for (uint32_t ctr = 0; ctr < full_counts; ctr ++) {
		// read 256 sectors;
		_do_read(start, 0, dataptr);
		dataptr += MAX_READ;
		number -= 256;
		start += 256;
	}

	_do_read(start, number, dataptr);
}

