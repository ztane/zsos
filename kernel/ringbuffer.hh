#ifndef __RINGBUFFER_HH__
#define __RINGBUFFER_HH__

#include "mutex.hh"
#include "waitqueue.hh"

#include "printk.h"
#include "scheduler.hh"

extern Scheduler scheduler;

template <class _T, class _MutexType = Mutex>
class RingBuffer {
	size_t nElems;
	_T * volatile elements;
	_MutexType mutex;	

	volatile size_t readPos;
	volatile size_t writePos;
	WaitQueue putQ;
	WaitQueue getQ;
public:
	RingBuffer(size_t size) {
		elements = new _T[size];
		readPos = writePos = 0;
		nElems = size;
	}
	void get(_T& to) {
		while (1) {
			bool b = tryGet(to);
			if (! b) {
				getQ.addCurrentTask();
				scheduler.schedule();
			}
			else {
				break;
			}
		}
	}

	bool tryGet(_T& to) {
		mutex.lock();
		if (readPos != writePos) {
			to = elements[readPos];
			readPos = (readPos + 1) % nElems;

			if (! putQ.isEmpty()) {
				putQ.resumeFirst();
			}

			mutex.unlock();
			return true;
		}
		mutex.unlock();
		return false;
	}

	void put(const _T& to) { 
		while (1) {
			bool b = tryPut(to);
			if (! b) {
				putQ.addCurrentTask();
				scheduler.schedule();
			}
			else {
				return;
			}
		}
	}
	bool tryPut(const _T& to) { 
		mutex.lock();
		if ((writePos + 1) % nElems != readPos) {
			elements[writePos] = to;
			writePos = (writePos + 1) % nElems;

			if (! getQ.isEmpty()) {
				getQ.resumeFirst();
			}

			mutex.unlock();
			return true;
		}
		mutex.unlock();
		return false;
	}
};

#endif
