// -*- C++ -*-

#ifndef SCHEDULER_INC
#define SCHEDULER_INC 1

#include <cstdlib>
#include "task.hh"

//  0 - 15: HISR (Highlevel Interrupt Service Routine)
// 16 - 31: Realtime tasks
// 32 - 47: Normal tasks
#define PRIV_LEVELS 48

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
		void add_task(Task* task);
		void remove_task(Task* task);
		void inc_ticks();
		Task *getCurrentTask() {
			return current;
		}
		void schedule();

	private:
		void dispatchNew(Task *, Task *);
		bool scheduler_running;
		uint64_t n_ticks;
		Task *current;
		task_dir tasks[PRIV_LEVELS];
};

extern Scheduler scheduler;

#endif
