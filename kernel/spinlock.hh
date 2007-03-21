#ifndef __SPINLOCK_HH__
#define __SPINLOCK_HH__

#include "atomic.hh"
#include "interrupt.hh"

class SpinLock {
private:
	volatile Atomic  allowCount;
public:
	SpinLock() : allowCount(1) { }

	void lockIRQ(bool& savedFlag) volatile {
		bool rv = disableInterrupts();
		lock();
	}

	void lock() volatile {
		while (1) {
			bool rv = allowCount.decAndIsZero();
			if (rv) {
				// got lock! ;)
				break;
			}
			allowCount ++;
		}
	}

	bool tryLock() volatile {
		bool got = allowCount.decAndIsZero();

		// did not get...
		if (! got) {
			// release;
			allowCount ++;
		}

		return got;
	}

	bool tryLockIRQ(bool& savedFlag) volatile {
		bool clied = disableInterrupts();
		bool rv = tryLock();
		if (! rv) 
			enableInterruptsIf(clied);
		else {
			savedFlag = clied;
			return rv;
		}		
	}
	
	void unlockIRQ(bool savedFlag) volatile {
		allowCount ++;
		enableInterruptsIf(savedFlag);
	}
	
	void unlock() volatile {
		allowCount ++;
	}
};

#endif
