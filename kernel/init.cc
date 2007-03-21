#include <iostream>
#include <cstdlib>

#include "init.hh"


namespace init {

static Init *list[NUM_LIST] = { 0 };

Init::Init(int when)
{
	next = list[when];
	list[when] = this;
}

Init::~Init() { } // dummy destructor

void run(int when)
{
	if (list[when] == 0) {
		kout << "init: warning: tried to call init::run() with nothing to do!" << endl;
		return;
	}

	kout << "Running init... ";

	while (list[when]) {
		list[when]->init();
		list[when] = list[when]->next;
	}

	kout << "done" << endl;
}

};
