#include "consoledriver.hh"
#include "kernel/fs/filedescriptor.hh"
#include "kernel/panic.hh"
#include "initial_vga.h"
#include "keyboard.hh"

ErrnoCode ConsoleDriver::open(int mode, FileDescriptor*& fd) {
	FileDescriptor::open(*this, mode, fd);
        return NOERROR;
}

ErrnoCode ConsoleDriver::read(void *buf, size_t amount, FileOffset offset, size_t& read) {
        read = (size_t)readKeyBuffer((char *)buf, amount);
        return NOERROR;
}

ErrnoCode ConsoleDriver::write(const void *buf, size_t amount, FileOffset offset, size_t& written) {
	// offset ignored.

	vga_buf_write(buf, amount);
	written = amount;
	return NOERROR;
}

ConsoleDriver the_driver;

FileLike *getConsoleDriver() {
	return static_cast<FileLike *>(&the_driver);
}

ConsoleDriver::ConsoleDriver() {
}
