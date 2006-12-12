#include "memarea.hh"

MemoryArea    DMAMemory(global_page_frame_table, 0, 0x1000);
MemoryArea NormalMemory(global_page_frame_table, 0x1000,   0x40000 -  0x1000);
MemoryArea   HighMemory(global_page_frame_table, 0x40000,  0x100000 - 0x40000);
