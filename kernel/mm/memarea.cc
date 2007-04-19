#include "memarea.hh"

MemoryArea    DMAMemory(page_frames, 0,        0x1000);
MemoryArea NormalMemory(page_frames, 0x1000,   0x30000 -  0x1000);
MemoryArea   HighMemory(page_frames, 0x30000,  0x100000 - 0x30000);
