#include <iostream>
#include "ide-disk.hh"


namespace IDE {

IDEDrive::IDEDrive() { }
IDEDrive::~IDEDrive() { }

int IDEDrive::init()
{
	kout << "IDEDrive got init()" << endl;
	return 0;
}

};
