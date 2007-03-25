// -*- C++ -*-

#ifndef USERTASK_HH
#define USERTASK_HH

#include "task.hh"

class UserTask : public Task {

protected:
       	uint32_t ustack; //stacktop of user stack
        uint32_t cr3;

	uint8_t  kernel_stack[4096];
	uint8_t  user_stack[16384];

public:
	UserTask(const char *name, State state = READY, int priority = NORMAL_LOW);	
	virtual ~UserTask();

	void initialize(void *entry_point);

	virtual void enable_io();
	virtual void dispatch(uint32_t *saved_eip);
	virtual void terminate();

	friend class Scheduler;

private:
        UserTask(const UserTask& t) : Task(t) { }
};

#endif
