#include "kernel/ktasks/softirq.hh"
#include "kernel/kerneltask.hh"
#include "kernel/refcount.hh"
#include "kernel/semaphore.hh"


typedef void (*SOFTIRQ_ROUTINE)(int vector);

static volatile SOFTIRQ_ROUTINE vectors[numSoftIrqVectors];
static volatile Atomic 		activationCount[numSoftIrqVectors];
static volatile Semaphore<int> 	softIrqSem;


bool registerSoftIrq(int vector, SOFTIRQ_ROUTINE routine) {
	if (vector < 0 || vector >= numSoftIrqVectors 
		|| vectors[vector] != 0)
			return false;
	
	activationCount[vector] = 0;
	vectors[vector] = routine;
	return true;
}

bool unregisterSoftIrq(int vector) {
	if (vector < 0 || vector >= numSoftIrqVectors 
		|| vectors[vector] == 0)
			return false;
	
	vectors[vector] = 0;
	activationCount[vector] = 0;
	return true;
}

bool triggerSoftIrq(int vector) {
	if (vector < 0 || vector >= numSoftIrqVectors)
		return false;

	activationCount[vector] ++;
	softIrqSem.post();
	return true; 
}

void softIrqTaskRoutine(void *param) {
	while (1) {
		softIrqSem.wait();
		for (int i = 0; i < numSoftIrqVectors; i ++) {
			if (activationCount[i] > 0) {
				activationCount[i] --;

				volatile SOFTIRQ_ROUTINE r = vectors[i];
				if (r) 
					r(i);

				break;
			} 
		}
	}
}

KernelTask softIrqTask("softirqd", Task::READY, Task::HISR_HIGH);

extern Scheduler scheduler;

void initSoftIrq() {
	softIrqTask.initialize(softIrqTaskRoutine, 0);
	scheduler.addTask(&softIrqTask);
}
