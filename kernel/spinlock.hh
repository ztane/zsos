#ifndef __SPINLOCK_HH__
#define __SPINLOCK_HH__

#include "atomic.hh"
#include "interrupt.hh"

class SpinLock {
private:
	volatile Atomic  allowCount;
public:
	SpinLock() : allowCount(1) { }

	void lockIRQ(bool& savedFlag) {
		bool rv = disableInterrupts();
		lock();
	}

	void lock() {
		while (1) {
			bool rv = allowCount.decAndIsZero();
			if (rv) {
				// got lock! ;)
				break;
			}
			allowCount ++;
		}
	}

	bool tryLock() {
		bool got = allowCount.decAndIsZero();

		// did not get...
		if (! got) {
			// release;
			allowCount ++;
		}

		return got;
	}

	bool tryLockIRQ(bool& savedFlag) {
		bool clied = disableInterrupts();
		bool rv = tryLock();
		if (! rv) 
			enableInterruptsIf(clied);
		else {
			savedFlag = clied;
			return rv;
		}		
	}
	
	void unlockIRQ(bool savedFlag) {
		allowCount ++;
		enableInterruptsIf(savedFlag);
	}
	
	void unlock() {
		allowCount ++;
	}
};

#endif
