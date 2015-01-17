#include "bits.hh"
#include <kernel/mm/memmap.hh>

uint8_t elfMagic[16] = "\177ELF\x1\x1\x1\0\0\0\0\0\0\0\0";


bool ElfExeHeader::initializeMemMapAreas(MemMap *m) {
        return true;
}
