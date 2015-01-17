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
	uint32_t entryPointer;
        bool     fpu_used;
	ZsosExeHeader *header;
        ElfExeHeader *elfHeader;
	VirtAddr baseAddress;
        uint8_t  fpu_state[512+16];
	uint8_t  kernel_stack[4096];

        inline uint8_t *getFpuStatePtr() const {
            // ensure it is aligned at 16
            return (uint8_t*)(((uint32_t)fpu_state + 15) & ~0xF);
        }

        void buildInitialKernelStack();
	void setUserStackPointer(uint32_t);
	void setEntryPointer(uint32_t);
        void initializeUserStack();
public:
	void buildMaps();
	UserTask(const char *name, State state = READY, int priority = NORMAL_LOW);
	virtual ~UserTask();

	void initialize(ZsosExeHeader *hdr);
	void initialize(ElfExeHeader *hdr);

	virtual void enable_io();
	virtual void dispatch(uint32_t *savedEip);
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
