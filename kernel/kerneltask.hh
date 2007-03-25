// -*- C++ -*-

#ifndef KERNELTASK_HH
#define KERNELTASK_HH

#include "task.hh"

class KernelTask : public Task {

public:
	KernelTask(const char *name, State state = READY, int priority = NORMAL_LOW);

	virtual ~KernelTask();

	void initialize(void (*entry_point)(void *data), void *parm);

	virtual void dispatch(uint32_t *saved_eip);
	virtual void terminate();

	friend class Scheduler;

private:
	uint8_t *kernelStack;
	uint32_t kernelStackSize;
	KernelTask(const KernelTask& t) : Task(t) { }

};

#endif
