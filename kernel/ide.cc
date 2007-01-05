#include "init.hh"
#include "ide.hh"


namespace IDE {

IDEController::IDEController()
{
	kout << "IDEController constructed" << endl;
	init::setup(this);
}

int IDEController::init()
{
	kout << "IDEController got init()" << endl;
	return 0;
}

IDEController controller; // static driver

};
