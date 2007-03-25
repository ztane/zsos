// -*- C++ -*-

#ifndef IDE_INC
#define IDE_INC 1

#include <iostream>
#include <cstdlib>
#include "kernel/printk.h"
#include "kernel/init.hh"
#include "kernel/pci.hh"
#include "kernel/ide-disk.hh"

namespace ide {

const int MAX_DRIVES = 2;
const int MAX_INTERFACES = 2;

class IdeInterface {
private:
	char name[8];

	IdeDrive drives[MAX_DRIVES];
public:
	IdeInterface();
	~IdeInterface();

	int init(int ifnum); // called by IDEController
};

class IdeController : public Init {
private:
	IdeInterface ifs[MAX_INTERFACES];
	PCI::PCIDevice pci; // PCI IDE controller?
public:
	IdeController();
	~IdeController();

	int init();
};

};

#endif
