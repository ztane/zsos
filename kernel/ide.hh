// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include "interrupt.hh"
#include "port.h"
#include "blockdevice.hh"
#include "pci.hh"

// port address offsets for Command Block registers
const unsigned short IDE_REG_BASE = 0x00;
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

// port address offsets for Control Block registers
const unsigned short IDE_ALT_STATUS	= 0x00;	// read-only
const unsigned short IDE_DEV_CTRL	= 0x00;	// write-only
const unsigned short IDE_DRV_ADDR	= 0x01;	// read-only OBSOLETE



#endif
