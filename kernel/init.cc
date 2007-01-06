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
	if (done)
		return;

	Init *ptr = list;
	while (ptr) {
		ptr->init();
		ptr = ptr->next;
	}

	// run once
	done = 1;
}

};
