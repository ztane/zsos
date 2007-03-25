#include <iostream>
#include <cstdlib>
#include "kernel/init.hh"

static Init *list[Init::LIST_MAX];

Init::Init(Init::Time when)
{
	next = list[when];
	list[when] = this;
}

Init::~Init() {}

void Init::run(Init::Time when)
{
	if (list[when] == 0) {
		kout << "init: warning: called runInit() with nothing to do!" << endl;
		return;
	}

	kout << "Running init... ";

	while (list[when]) {
		list[when]->init();
		list[when] = list[when]->next;
	}

	kout << "done" << endl;
}
