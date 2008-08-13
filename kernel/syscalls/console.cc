#include "kernel/syscall.hh"
#include "kernel/initial_vga.h"
#include "kernel/printk.h"
#include "kernel/errnocode.hh"
#include "kernel/fs/filedescriptor.hh"

#include <iostream>
#include <scheduler.hh>

extern Scheduler scheduler;

SYSCALL(write_character)
{
	kout << (char)r.ebx;
        SYSCALL_RETURN(0);
}

SYSCALL(write)
{
	int fdn = r.ebx;
	void *ptr = (void*)r.ecx;
	size_t count = (size_t)r.edx;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	int rc;

	// FIXME: check for buffer for reading!

	// Not open?
	if (! fd)
		rc = EBADF;
	else
		rc = fd->write(ptr, count);

	SYSCALL_RETURN(rc);
}

SYSCALL(read)
{
	int fdn = r.ebx;
	void *ptr = (void*)r.ecx;
	size_t count = (size_t)r.edx;

	Task *current = scheduler.getCurrentTask();
	FileDescriptor *fd = current->getFileDescriptor(fdn);

	int rc;

	// FIXME: check buffer for writing!

	// Not open?
	if (! fd)
		rc = EBADF;
	else
		rc = fd->read(ptr, count);

        SYSCALL_RETURN(rc);
}

SYSCALL(hello_world)
{
	SYSCALL_RETURN(0);
}
