// -*- C++ -*-
// this will be the base class for all character device drivers
// 
// CharDev is abstract class
// 
// All chardev drivers should be inherited from this interface,
// or from something like this.
//

#ifndef CHARDEVICE_INC
#define CHARDEVICE_INC 1

#include <cstdlib>

class CharDev {
	public:
//		At the moment do nothing here. Drivers are _NOT_
//		supposed to quit. (Possible module unloading in
//		some distant future?)

		virtual ~CharDev() {   }

//		This is an abstract class - interface if you may.
//		No objects should be created directly from this.

		virtual size_t read(void *vbuf, size_t len) = 0;
		virtual size_t write(const void *vbuf, size_t len) = 0;
		virtual int    open() = 0;
		virtual int    close() = 0;
	private:
// 		Doesn't use any data as of now.
// 		Possibly in the future some bookkeeping
// 		data etc may reside here.
};

#endif
