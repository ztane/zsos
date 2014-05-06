#include "kernel/syscall.hh"
// #include "kernel/initial_vga.h"
#include "kernel/printk.h"
#include "kernel/errnocode.hh"
#include "kernel/fs/filedescriptor.hh"

#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;

SYSCALL(write_character)
{
	kout << (char)r.arg0;
        SYSCALL_RETURN(0);
}

SYSCALL(write)
{
	int fdn = r.arg0;
	void *ptr = (void*)r.arg1;
	size_t count = (size_t)r.arg2;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	int rc = 0;

	// FIXME: check for buffer for reading!

	// Not open?
	if (! fd) {
		rc = EBADF;
	}
	else {
		size_t written;
		rc = -fd->write(ptr, count, written);
		if (rc == 0) {
			rc = written;
		}
	}

	SYSCALL_RETURN(rc);
}

SYSCALL(read)
{
	int fdn = r.arg0;
	void *ptr = (void*)r.arg1;
	size_t count = (size_t)r.arg2;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	int rc = 0;

	// FIXME: check buffer for writing!

	// Not open?
	if (! fd)
		rc = EBADF;
	else {
		size_t read;
		rc -= fd->read(ptr, count, read);

		if (rc == 0) {
			rc = read;
		}
	}
        SYSCALL_RETURN(rc);
}
