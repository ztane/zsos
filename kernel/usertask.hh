// -*- C++ -*-

#ifndef USERTASK_HH
#define USERTASK_HH

#include "task.hh"
#include "kernel/exe/bits.hh"

class UserTask : public Task {

protected:
       	uint32_t ustack; //stacktop of user stack
        uint32_t cr3;
	uint8_t  kernel_stack[4096];
	
	ZsosExeHeader *header;

public:
	UserTask(const char *name, State state = READY, int priority = NORMAL_LOW);	
	virtual ~UserTask();

	void initialize(ZsosExeHeader *hdr);

	virtual void enable_io();
	virtual void dispatch(uint32_t *saved_eip);
	virtual void terminate();
	virtual bool handlePageFault(PageFaultInfo& f);
	virtual void* setBrk(void *newBrk);

	friend class Scheduler;

private:
        UserTask(const UserTask& t) : Task(t) { }
};

#endif
