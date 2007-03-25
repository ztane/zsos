// -*- C++ -*-

#ifndef INIT_VGA
#define INIT_VGA 1

#include <cstdlib>

class VgaDev
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


