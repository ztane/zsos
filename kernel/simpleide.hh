#ifndef __SIMPLEIDE__HH__
#define __SIMPLEIDE__HH__

void reset_ide_disk();
void do_read_ide(uint64_t start, uint32_t number, void *data);

#endif

