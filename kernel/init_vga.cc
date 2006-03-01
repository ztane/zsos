#include <stdlib.h>
#include "init_vga"

extern "C" void   init_vga_buffer();
extern "C" size_t vga_buf_write(const void *, size_t);

VgaDev::VgaDev() : CharDev()
{
	init_vga_buffer();
}

VgaDev::~VgaDev()
{

}

size_t VgaDev::read(void *vbuf, size_t len)
{
	return 0;
}

size_t VgaDev::write(const void *vbuf, size_t len)
{
	return vga_buf_write(vbuf, len);
}

int VgaDev::open()
{
	return 0;
}

int VgaDev::close()
{
	return 0;
}


