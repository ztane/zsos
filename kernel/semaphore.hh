#ifndef __SEMAPHORE_HH_INCLUDED__
#define __SEMAPHORE_HH_INCLUDED__

#include "spinlock.hh"
#include "waitqueue.hh"
#include "scheduler.hh"
#include "printk.h"

template<typename T>
class Semaphore {
private:
	SpinLock  lock;
	WaitQueue waitq;
        volatile T count;
public:
	Semaphore() {
		count = 0;
	}
	Semaphore(T val) {
		count = val;
	}
	
	operator T() {
		lock.lock();
		T rv = count;
		lock.unlock();
		return rv;
	}

	void post() volatile {
		extern Scheduler scheduler;
		lock.lock();
		if (! waitq.isEmpty()) {
			waitq.resumeFirst();
		}
		else {
			count ++;
		}
		lock.unlock();
	}

	void wait() volatile {
		lock.lock();
		if (count != 0) {
			count --;
			lock.unlock();
			return;
		}
		else {
			extern Scheduler scheduler;
			waitq.addCurrentTask();
			lock.unlock();
			scheduler.schedule();
		}
	}
};

#endif
