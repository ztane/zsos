// -*- C++ -*-

#ifndef IDE_PORTS_INC
#define IDE_PORTS_INC 1

namespace ide {

/*
static const uint16_t CMD_BASE_0	= 0x1f0;
static const uint16_t CTRL_BASE_0	= 0x3f6;
static const uint16_t CMD_BASE_1	= 0x170;
static const uint16_t CTRL_BASE_1	= 0x376;
*/

// port address offsets for Command Block registers
static const uint32_t DATA 	= 0; // read-write - data register
static const uint32_t ERROR 	= 1; // read-only - error register
static const uint32_t NSECTOR 	= 2; // read-write - sector count
static const uint32_t SECTOR 	= 3; // read-write - sector number (CHS mode)
static const uint32_t LCYL 	= 4; // read-write - cylinder low (CHS mode)
static const uint32_t HCYL 	= 5; // read-write - cylinder high (CHS mode)
static const uint32_t SELECT 	= 6; // read-write - drive/head
static const uint32_t STATUS 	= 7; // read-only - status register
static const uint32_t FEATURE 	= ERROR;  // write-only - Write Precompensation Cylinder divided by 4
static const uint32_t COMMAND 	= STATUS; // write-only - command register

static const uint32_t LBCOUNT 	= SECTOR; //   7-0 in LBA28
static const uint32_t MBCOUNT 	= LCYL;   //  15-8 in LBA28
static const uint32_t HBCOUNT 	= HCYL;   // 23-16 in LBA28, 27-24 go in low half of SELECT

// port address offsets for Control Block registers
static const uint32_t ALTSTATUS = 0; // read-only
static const uint32_t CONTROL	= ALTSTATUS; // write-only

};

#endif
