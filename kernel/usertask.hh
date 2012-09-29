// -*- C++ -*-

#ifndef USERTASK_HH
#define USERTASK_HH

#include "task.hh"
#include "kernel/exe/bits.hh"
#include "kernel/fs/filedescriptor.hh"

static const int MAX_FILEDES = 64;

class UserTask : public Task {

protected:
       	uint32_t ustack; //stacktop of user stack
        uint32_t cr3;
        bool     fpu_used;
	ZsosExeHeader *header;
        uint8_t  fpu_state[512+16];
	uint8_t  kernel_stack[4096];

        inline uint8_t *getFpuStatePtr() const {
            // ensure it is aligned at 16
            return (uint8_t*)((uint32_t)fpu_state + 15 & ~0xF);
        }
public:
	UserTask(const char *name, State state = READY, int priority = NORMAL_LOW);
	virtual ~UserTask();

	void initialize(ZsosExeHeader *hdr);
	void initialize(ElfExeHeader *hdr);

	virtual void enable_io();
	virtual void dispatch(uint32_t *saved_eip);
	virtual void terminate();
	virtual bool handlePageFault(PageFaultInfo& f);
	virtual void* setBrk(void *newBrk);
        virtual void handleNMException();
        virtual void prepareContextSwitch();

	friend class Scheduler;

private:
        UserTask(const UserTask& t) : Task(t) { }
};

#endif
