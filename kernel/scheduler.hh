// -*- C++ -*-

#ifndef SCHEDULER_INC
#define SCHEDULER_INC 1

#include <cstdlib>
#include "tasking.hh"

//  0 - 15: HISR (Highlevel Interrupt Service Routine)
// 16 - 31: Realtime tasks
// 32 - 47: Normal tasks
#define PRIV_LEVELS 48

// NOTE: turn into a class if requires complex operations!
typedef struct _process_dir
{
	Process *first;
	Process *last;
} process_dir;

class Scheduler
{
	public:
		Scheduler();
		//~Scheduler();
		
		void schedule();
		void add_process(Process *process);
		void remove_process(Process *process);
		void inc_ticks();
		Process *getCurrentTask() {
			return current;
		}
	private:
		void dispatchNew(Process*, Process*);
		bool scheduler_running;
		uint64_t n_ticks;
		Process *current;
		process_dir tasks[PRIV_LEVELS];
};

extern Scheduler scheduler;

#endif
