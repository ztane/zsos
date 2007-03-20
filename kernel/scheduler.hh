// -*- C++ -*-

#ifndef SCHEDULER_INC
#define SCHEDULER_INC 1

#include <cstdlib>
#include "task.hh"

//  0 - 15: HISR (Highlevel Interrupt Service Routine)
// 16 - 31: Realtime tasks
// 32 - 47: Normal tasks
// 48     : idle...
#define PRIV_LEVELS 49

// NOTE: turn into a class if requires complex operations!
typedef struct _task_dir
{
	Task *first;
	Task *last;
} task_dir;

class Scheduler
{
	public:
		Scheduler();
		//~Scheduler();
		
		void needsScheduling();
		void addTask(Task* task);
		void removeTask(Task* task);
		void incTicks();
		Task *getCurrentTask() {
			return current;
		}
		void schedule();

	private:
		void dispatchNew(Task *, Task *);
		bool schedulerRunning;
		uint64_t nTicks;
		Task *current;
		task_dir tasks[PRIV_LEVELS];
};

extern Scheduler scheduler;

#endif
