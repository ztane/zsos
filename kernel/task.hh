// -*- C++ -*-

#ifndef TASKING_H
#define TASKING_H

#include <stdint.h>
#include <ostypes>

class Task {

public:
	enum State {
		READY = 0,
		RUNNING = 1,
		BLOCKED = 2,
		SUSPENDED = 4,
		BLOCKED_AND_SUSPENDED = 6,
		TERMINATED = 8
	};

	enum Priority {
		HISR_HIGH     = 0,
		HISR_LOW      = 15,
		REALTIME_HIGH = 16,
		REALTIME_LOW  = 31,
		NORMAL_HIGH = 32,
		NORMAL_LOW  = 47,
		IDLE 	 = 48
	};

protected:
	char name[32];
	bool isNew;

	int current_priority;
	int timeslice;
	pid_t process_id;

	State current_state;

	Task *next;
	Task *previous;

        unsigned int  kstack; //stacktop of kernel stack
	unsigned int  esp;    //actual position of esp
	unsigned int  padding;
	unsigned char kernel_stack[4096];

public:
	Task(const char *name, State initial, int priority);
	virtual ~Task() { };

	State getCurrentState() const {
		return current_state;
	}

	void setCurrentState(State new_state) {
		current_state = new_state;	
	}

	int getCurrentPriority() const {
		return current_priority;
	}

	void setCurrentPriority(int priority) {
		if (priority > IDLE)
			priority = NORMAL_LOW;

		if (priority < 0)
			priority = NORMAL_HIGH;

		current_priority = priority;
	}

	void setProcessId(uint32_t pid) {
		process_id = pid;
	}

	uint32_t getProcessId() const {
		return process_id;
	}

	Task *getNext() const {
		return next;
	}

	void setNext(Task *n) {
		next = n;
	}

	Task *getPrevious() const {
		return previous;
	}

	void setPrevious(Task *p) {
		previous = p;
	}

	virtual bool handlePageFault(uint32_t address);
	virtual void dispatch(uint32_t* saved_eip) = 0;
	virtual void terminate() = 0;

	friend class Scheduler;
};

#endif
