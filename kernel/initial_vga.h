#ifndef __INITIAL_VGA_H__
#define __INITIAL_VGA_H__

#include "stdlib.h"

extern void   init_vga_buffer();
extern size_t vga_buf_write(const void *, size_t);

#endif
