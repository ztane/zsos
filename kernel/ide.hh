// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include "kernel/init.hh"
#include "kernel/ide-drive.hh"
#include "kernel/ide-disk.hh"
#include "kernel/ringbuffer.hh"
#include "kernel/waitqueue.hh"

namespace ide {

static const int    MAX_DRIVES       = 2;
static const int    MAX_INTERFACES   = 2;
static const size_t MAX_IDE_REQUESTS = 8; // make tunable




class IdeInterface {
private:
	const int ifnum;
	IdeDrive *drives[MAX_DRIVES];
	// FIXME: Replace this ringbuffer with a linked list
	RingBuffer<struct ide_request_t> *requests;
	WaitQueue wq;
public:
	IdeInterface(int ifnum);
	~IdeInterface();

	void add_request(int rw, int drive, void *data, unsigned long long block, size_t count);
	void softirq_handler();
};




class IdeController : public Init {
private:
	int softirq_vectors[MAX_INTERFACES];
public:
	IdeInterface *ifs[MAX_INTERFACES];

	IdeController();
	~IdeController();

	int init();

	friend void irq_handler(int ifnum);
	friend void if0_softirq_handler(int vector);
	friend void if1_softirq_handler(int vector);
};




void irq_handler(int ifnum);

extern IdeController controller;

};

#endif
