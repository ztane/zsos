// -*- C++ -*-

#ifndef USERTASK_HH
#define USERTASK_HH

#include "task.hh"

class UserTask : public Task {

protected:
        unsigned int ustack; //stacktop of user stack
        unsigned int cr3;

	unsigned int padding2;
	unsigned char user_stack[16384];

public:
	UserTask(const char *name);	
	virtual ~UserTask();

	void initialize(void *entry_point);

	virtual void enable_io();
	virtual void dispatch(uint32_t *saved_eip);

	friend class Scheduler;
};

#endif
