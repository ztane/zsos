#include "init.hh"
#include "ide.hh"


namespace IDE {

IDEController::IDEController() : init::Init()
{
	kout << "IDEController constructed" << endl;
	init::setup(this);
}

IDEController::~IDEController()
{

}

int IDEController::init()
{
	kout << "IDEController got init()" << endl;
	return 0;
}

void IDEController::dummy() { printk("PERKELE\n"); }

IDEController controller; // static driver

};
