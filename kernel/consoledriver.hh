#ifndef CONSOLEDRIVER_HH_INCLUDED
#define CONSOLEDRIVER_HH_INCLUDED

#include "kernel/fs/filelike.hh"

class VgaBuffer;

class ConsoleDriver: public FileLike {
private:
	VgaBuffer *buffer;
public:
	ConsoleDriver();
	virtual bool isSeekable() { return false; };

        virtual ErrnoCode open(int mode, FileDescriptor*& fd);
        virtual ErrnoCode read(void *buf, size_t amount, FileOffset offset);
        virtual ErrnoCode write(const void *buf, size_t amount, FileOffset offset);
};

FileLike *getConsoleDriver();

#endif
