#include "kernel/ide.hh"

namespace ide {

IdeController::IdeController() : Init(EARLY) {}

IdeController::~IdeController() {}

int IdeController::init()
{
	for (int i = 0; i < MAX_INTERFACES; i ++)
		ifs[i].init(i);

	return 0;
}

IdeInterface::IdeInterface() {}
IdeInterface::~IdeInterface() {}

int IdeInterface::init(int ifnum)
{
	for (int i = 0; i < MAX_DRIVES; i ++)
		drives[i].init(ifnum, i);

	return 0;
}

IdeController controller; // static driver

};
