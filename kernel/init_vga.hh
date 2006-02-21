#ifndef INIT_VGA_HH
#define INIT_VGA_HH

#include <stdlib.h>
#include "chardevice.hh"

class VgaDev : public CharDev
{
	public:
		VgaDev();
		virtual ~VgaDev();

		virtual size_t read(void *vbuf, size_t len);
		virtual size_t write(const void *vbuf, size_t len);
		virtual int open();
		virtual int close();
	private:
};

#endif


