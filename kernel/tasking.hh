// -*- C++ -*-

#ifndef TASKING_H
#define TASKING_H

#include <cstdlib>
#include <ostypes>
#include <iostream>
#include <printk.h>

class TSSContents{
public:
	unsigned short backlink, __blh;
       	unsigned int   esp0;
	unsigned short ss0, __ss0h;
        unsigned int   esp1;
	unsigned short ss1, __ss1h;
        unsigned int   esp2;
	unsigned short ss2, __ss2h;
	unsigned int   cr3;
	unsigned int   eip;
	unsigned int   eflags;
	unsigned int   eax, ecx, edx, ebx;
       	unsigned int   esp, ebp, esi, edi;
       	unsigned short es, __esh;
        unsigned short cs, __csh;
        unsigned short ss, __ssh;
        unsigned short ds, __dsh;
        unsigned short fs, __fsh;
        unsigned short gs, __gsh;
	unsigned short ldt, __ldth;
	unsigned short trace, bitmap;
	void setup();
} __attribute__((packed));


class Process {

public:
	enum ProcessState {
		READY = 0,
		RUNNING = 1,
		BLOCKED = 2,
		SUSPENDED = 4,
		BLOCKED_AND_SUSPENDED = 6
	};

protected:
	unsigned int esp;    //actual position of esp
        unsigned int kstack; //stacktop of kernel stack
        unsigned int ustack; //stacktop of user stack
        unsigned int cr3;

	char name[32];
	bool isNew;
	unsigned int padding;
	unsigned char kernel_stack[4096];
	unsigned int padding2;
	unsigned char user_stack[16384];
		
	int current_priority;
	int timeslice;
	pid_t process_id;

	ProcessState current_state;

	Process *next;
	Process *previous;

public:
	Process(char *name);
	
	void initialize(void *entry_point);

	ProcessState getCurrentState() {
		return current_state;
	}

	void setCurrentState(ProcessState new_state) {
		current_state = new_state;	
	}

	int getCurrentPriority() {
		return current_priority;
	}

	void setCurrentPriority(int priority) {
		current_priority = priority;
	}

	void setProcessId(uint32_t pid) {
		process_id = pid;
	}

	uint32_t getProcessId() {
		return process_id;
	}

	Process *getNext() {
		return next;
	}

	void setNext(Process *n) {
		next = n;
	}

	Process *getPrevious() {
		return previous;
	}

	void setPrevious(Process *p) {
		previous = p;
	}

	void enable_io();

	bool handlePageFault(uint32_t address) {
		printk("Process %d had a pfault at %p\n", process_id, address);
		__asm__ __volatile__ ("hlt");
		return true;
	}

	friend class Scheduler;

};



extern TSSContents TSS_Segment;

#endif
