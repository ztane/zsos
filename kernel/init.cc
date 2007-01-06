#include <iostream>
#include <cstdlib>

#include "init.hh"


namespace init {

Init::~Init() { } // dummy destructor

static int done = 0;
static Init *list = NULL;

void setup(Init *ptr)
{
	ptr->next = list;
	list = ptr;
}

void run()
{
	if (list == NULL) {
		// can overflow (on INT32_MAX * 2 attempts...)
		kout << "init: warning: tried to call init::run() with nothing to do!" << endl;
		return;
	}
	kout << "Running init... ";
	// unwind list:
	while (list) {
		list->init();
		list = list->next;
	}
	kout << "done" << endl;
}

};
