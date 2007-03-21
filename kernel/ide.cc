#include "init.hh"
#include "ide.hh"


namespace IDE {

IDEController::IDEController() : init::Init() { }

IDEController::~IDEController() { }

int IDEController::init()
{
	kout << "IDEController got init()" << endl;
	for (int i = 0; i < MAX_INTERFACES; i ++)
		ifs[i].init();

	return 0;
}

IDEInterface::IDEInterface() { }
IDEInterface::~IDEInterface() { }

int IDEInterface::init()
{
	kout << "IDEInterface got init()" << endl;
	for (int i = 0; i < MAX_DRIVES; i ++)
		drives[i].init();

	return 0;
}

IDEController controller; // static driver

};
