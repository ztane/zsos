// -*- C++ -*-

#ifndef TASKING_H
#define TASKING_H

#include <stdint.h>
#include <ostypes>
#include "kernel/paging.hh"
#include "kernel/mm/memmap.hh"


// Avoid including...
class FileDescriptor;

class Task {

public:
        static const int MAX_FILEDES = 64;

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

	MemMap *memmap;

        unsigned int  kstack; //stacktop of kernel stack
	unsigned int  esp;    //actual position of esp

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

	void switchContexts(uint32_t*);

	virtual bool handlePageFault(PageFaultInfo& f);
	virtual void dispatch(uint32_t* saved_eip) = 0;
        virtual void prepareContextSwitch() { }
	virtual void terminate() = 0;
	virtual void *setBrk(void *newBrk);
        virtual void handleNMException();

	friend class Scheduler;

	// FIXME: SHOULD HAVE A PROCESS CLASS REALLY?!
        FileDescriptor *fileDescriptors[MAX_FILEDES];
	FileDescriptor *getFileDescriptor(int number);
        bool setFileDescriptor(int number, FileDescriptor* value);
	int  findFreeFdSlot(FileDescriptor* value);
};

#endif
