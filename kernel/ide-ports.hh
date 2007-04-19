// -*- C++ -*-

#ifndef IDE_PORTS_INC
#define IDE_PORTS_INC 1

namespace ide {

/*
const uint16_t CMD_BASE_0	= 0x1f0;
const uint16_t CTRL_BASE_0	= 0x3f6;
const uint16_t CMD_BASE_1	= 0x170;
const uint16_t CTRL_BASE_1	= 0x376;
*/

// port address offsets for Command Block registers
const uint16_t DATA_OFFSET = 0; // read-write - data register
const uint16_t ERROR_OFFSET = 1; // read-only - error register
const uint16_t NSECTOR_OFFSET = 2; // read-write - sector count
const uint16_t SECTOR_OFFSET = 3; // read-write - sector number (CHS mode)
const uint16_t LCYL_OFFSET = 4; // read-write - cylinder low (CHS mode)
const uint16_t HCYL_OFFSET = 5; // read-write - cylinder high (CHS mode)
const uint16_t SELECT_OFFSET = 6; // read-write - drive/head
const uint16_t STATUS_OFFSET = 7; // read-only - status register
const uint16_t FEATURE_OFFSET = ERROR_OFFSET;  // write-only - Write Precompensation Cylinder divided by 4
const uint16_t COMMAND_OFFSET = STATUS_OFFSET; // write-only - command register

const uint16_t LBCOUNT_OFFSET = SECTOR_OFFSET; //   7-0 in LBA28
const uint16_t MBCOUNT_OFFSET = LCYL_OFFSET;   //  15-8 in LBA28
const uint16_t HBCOUNT_OFFSET = HCYL_OFFSET;   // 23-16 in LBA28, 27-24 go in low half of SELECT

// port address offsets for Control Block registers
const uint16_t ALTSTATUS_OFFSET = 0; // read-only
const uint16_t CONTROL_OFFSET = ALTSTATUS_OFFSET; // write-only

};

#endif