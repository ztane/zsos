// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include <string.h>
#include "kernel/init.hh"
#include "ide-drive.hh"
#include "ide-disk.hh"
#include "port.h"
#include "kernel/ringbuffer.hh"
#include "kernel/waitqueue.hh"

namespace ide {

static const int    MAX_DRIVES       = 2;
static const int    MAX_INTERFACES   = 2;
static const size_t MAX_IDE_REQUESTS = 8; // make tunable




class IdeInterface {
private:
        int       base;
        int       irq;
	bool      floating_bus;

	IdeDrive *drives[MAX_DRIVES];
	int       drives_count;
	int       selected_drive;

	// FIXME: Replace this ringbuffer with a linked list
	RingBuffer<struct ide_request_t> *requests;

	WaitQueue wq;

	void __select_drive(int num);

public:
	IdeInterface(int base, int irq);

	void add_request(int rw, int drive, void *data, unsigned long long block, size_t count);
	void softirq_handler();
};




class IdeController : public Init {
private:
	int softirq_vectors[MAX_INTERFACES];
public:
	IdeInterface *ifs[MAX_INTERFACES];

	IdeController();

	int init();

	friend void irq_handler(int ifnum);
	friend void if0_softirq_handler(int vector);
	friend void if1_softirq_handler(int vector);
};




void irq_handler(int ifnum);

extern IdeController controller;

};

#endif
