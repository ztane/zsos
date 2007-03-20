// -*- C++ -*-

#ifndef KERNELTASK_HH
#define KERNELTASK_HH

#include "task.hh"

class KernelTask : public Task {

protected:
        unsigned int ustack; //stacktop of user stack
	unsigned int padding2;
	unsigned char user_stack[16384];

public:
	KernelTask(const char *name);	
	virtual ~KernelTask();

	void initialize(void (*entry_point)(void *data));

	virtual void dispatch(uint32_t *saved_eip);

	friend class Scheduler;
};

#endif
