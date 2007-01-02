#ifndef __MUTEX_HH_INCLUDED__
#define __MUTEX_HH_INCLUDED__

#include "semaphore.hh"

class Mutex : private Semaphore<int> {
public:
	Mutex() : Semaphore<int>(1) { }
	void lock() {
		wait();
	}
	void unlock() {
		post();
	}
};

#endif
