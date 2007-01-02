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
        T count;
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

	void post() {
		extern Scheduler scheduler;
		lock.lock();
		if (! waitq.isEmpty()) {
			printk("NOT EMPTY\n");
			Process *task = waitq.extractFirst();
			printk("GOT PROCESS %d\n", task->getProcessId());
			task->setCurrentState(Process::READY);
			scheduler.add_process(task);
			printk("ADDED PROCESS!\n");
		}
		else {
			count ++;
		}
		lock.unlock();
	}

	void wait() {
		lock.lock();
		if (count != 0) {
			count --;
			lock.unlock();
			return;
		}
		else {
			extern Scheduler scheduler;
			Process *task = scheduler.getCurrentTask();
			waitq.addFirst(task);
			task->setCurrentState(Process::BLOCKED);

			lock.unlock();
			scheduler.schedule();
		}
	}
};

#endif
